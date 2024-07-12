pub(crate) mod world_matrix;

use godot::engine::Engine;
use spawn_weights_matrix::SpawnWeightsMatrix;
use strum::EnumCount;
use strum::IntoEnumIterator;
use strum::VariantNames;
use world_matrix::*;
use crate::beings::*;
use crate::formation_generation::*;
use crate::tiling::TileDto;
use crate::utils::matrix::DownScalingMatrix;
use crate::utils::safe_vec::SafeVec;
use crate::utils::uns_vec::UnsVec;
use godot::builtin::Dictionary;
use godot::engine::{INode2D, Node2D};
use godot::prelude::*;
use std::borrow::{Borrow, BorrowMut};
use std::collections::{HashMap, HashSet};
use std::fmt;
use std::hash::{Hash, Hasher};
#[derive(GodotClass)]
#[class(base=Node2D)]
struct RustTileMap {

  #[var] layer_count: u16,
  #[var] seed: i64,
  tile_unid_mapping: Vec<TileDto>,
  world_matrix: Option<WorldMatrix>,
  base: Base<Node2D>,
  world_size: UnsVec,
  //don't remove an entry directly
  being_loaded_tiles_map: HashMap<BeingUnid, HashSet<UnsVec>>,
  //don't reduce this directly
  tile_shared_loads_count: HashMap<UnsVec, i64>,
//-- beings section --
  beings_in_chunk_count: Option<DownScalingMatrix<u16>>,
  spawn_weights_matrix: Option<SpawnWeightsMatrix>,

}
#[godot_api]
impl INode2D for RustTileMap {
  fn init(base: Base<Node2D>) -> Self {
    Self {
      base,
      layer_count: TileZLevel::COUNT as u16,
      seed: 0,
      world_matrix: None,
      tile_unid_mapping: Vec::new(),
      world_size: UnsVec::ZERO,
      being_loaded_tiles_map: Default::default(),
      tile_shared_loads_count: Default::default(),
      spawn_weights_matrix: None,
      beings_in_chunk_count: None
    }
  }

  fn ready(&mut self) {

    let layer_names: Array<StringName> = TileZLevel::VARIANTS
      .iter().map(|&name| StringName::from(name)).collect();

    let layer_names: [Variant; 1] = [layer_names.to_variant()];
    
    self.base_mut().call( "_add_tile_map_layers".into(), &layer_names);
  }
}
#[godot_api]
impl RustTileMap {
  const MACROSCOPIC_SPAWNING_CHUNK_SIZE: u8 = 15;
  const BEING_LIMIT_PER_MACROSCOPIC_SPAWNING_CHUNK: u16 = 200;  

  #[func]
  fn generate_world_matrix(&mut self, size: Vector2i, tiles: Array<Gd<Tile>>) {
    
    assert!(tiles.len() < TileUnid::NULL.0 as usize);
    
    self.tile_unid_mapping.extend(tiles.iter_shared()
      .enumerate() 
      .map(|(i, mut tile)| {
        tile.bind_mut().unid = Some(TileUnid{0: i as u16}); 
        tile.into()
      })
    );
    let size: UnsVec = size.try_into().expect("passed arg size: Vector2i is negative");

    const MIN_SIZE: u32 = 500;
    if size.all_bigger_than_min(MIN_SIZE).is_err(){
      godot_error!("world size is too small, must be at least {MIN_SIZE}X{MIN_SIZE}");
    }

    self.world_matrix = Some(WorldMatrix::new(size));     
    self.world_size = size;
    self.spawn_weights_matrix = Some(SpawnWeightsMatrix::new(size, 3));
  }
  #[func]
  fn generate_formation(&mut self, formation: FormGenEnum, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) -> bool{
    
    let now = std::time::Instant::now();
    generate(self.world_matrix.as_mut().expect("world matrix needs to be generated before formation (call generate_world_matrix first)"), formation, origin, size, tile_selection, seed, data);
    godot_print!("time taken to generate: {:.2?}", now.elapsed());
    true
  }

  #[func]
  fn load_tiles_around(&mut self, _being_coords: Vector2i, chunk_size: Vector2i, being_unid: i64) {

    let chunk_size = SafeVec::from(chunk_size).all_bigger_than_min(10).expect("chunk size is smaller than minimum 10");
    let being_unid = BeingUnid(being_unid);
    
    let being_coords: SafeVec = _being_coords.into();

    let world_size = self.world_size;

    for chunk_coord in (-chunk_size.lef as i32/2..chunk_size.lef as i32/2).flat_map(|i| (-chunk_size.right as i32/2..chunk_size.right as i32/2).map(move |j| (i,j)))
      .map(|vec| SafeVec::from(vec) + being_coords)
      .filter(|vec| vec.is_non_negative())
      .map(|vec|unsafe{UnsVec::try_from(vec).unwrap_unchecked()})
      .filter(|vec| vec.is_strictly_smaller_than(world_size)){unsafe{
        if ! self.tile_shared_loads_count.contains_key(&chunk_coord) {
          
          self.tile_shared_loads_count.insert(chunk_coord, 1);

          let tiles = self.world_matrix.as_ref().unwrap_unchecked()[chunk_coord];
          tiles.iter().filter(|&&unid| unid != TileUnid::NULL).for_each(|&unid| self.set_cell(unid, chunk_coord));
        } 
        else if !self.being_loaded_tiles_map.get(&being_unid).map_or(false, |set| set.contains(&chunk_coord)) {
          *self.tile_shared_loads_count.get_mut(&chunk_coord).unwrap_unchecked() += 1;
        }
        self.being_loaded_tiles_map.entry(being_unid).or_insert_with(|| HashSet::with_capacity(chunk_size.area()*3/2)).insert(chunk_coord);
          
      }}
    self.unload_excess_tiles(being_coords, chunk_size.into(), being_unid);
  }
  fn set_cell(&mut self, unid: TileUnid, matrix_coord: UnsVec) {
    let tile: &TileDto = self.tile_nid_mapping().get(unid.0 as usize).expect(format!("tile mapped to {unid} not found").as_str());
    unsafe{
      let tile_z_level: i32 = tile.z_level as i32;
      let atlas_origin_position: Vector2i = (*tile).origin_position;
      let atlas_origin_position_offset: Vector2i = matrix_coord.mod_unsv((*tile).modulo_tiling_area).into(); 

      let args: [Variant; 4] = 
        [Into::<Vector2i>::into(matrix_coord).to_variant(), ((*tile).source_atlas).to_variant(),
        (atlas_origin_position+atlas_origin_position_offset).to_variant(), (*tile).alternative_id.to_variant()];

      // TODO: METER  NUEVA TILEMAPLAYER SI NO TA. GUARDAR SU REF EN UN DICT CON KEY=TILEID
      self.base_mut().get_child(tile_z_level).unwrap_unchecked().call("set_cell".into(), &args);
    }
  }
  fn unload_excess_tiles(&mut self, being_coords: SafeVec, chunk_size: UnsVec, being_unid: BeingUnid) {unsafe {
    let self_ptr: *mut Self = self as *mut _;

    let loaded_tiles = self.being_loaded_tiles_map.get_mut(&being_unid).unwrap_unchecked();

    loaded_tiles.retain(|&tile_coord| {
      let keep: bool = chunk_size.within_bounds_centered(SafeVec::from(tile_coord) - being_coords);
      
      if keep == false {(*self_ptr).decrement_shared_loads_count(tile_coord);}
      keep
    });
  }}
  #[signal] pub fn tile_unloaded(coords: Vector2i);
  fn decrement_shared_loads_count(&mut self, tile_coord: UnsVec) {
    if let Some(&mut mut count) = self.tile_shared_loads_count.get_mut(tile_coord.borrow()) {
      if count > 0 {count -= 1;}
      
      if count == 0 {
        self.tile_shared_loads_count.remove(tile_coord.borrow());
        let tile_coord: &[Variant; 1] = &[Into::<Vector2i>::into(tile_coord).to_variant()];
        for layer_i in 0..self.layer_count {
          unsafe{self.base_mut().get_child(layer_i as i32).unwrap_unchecked().call("erase_cell".into(), tile_coord);}
        }
        self.base_mut().emit_signal("tile_unloaded".into(), tile_coord);
      }
    }
  }
  #[func]
  fn untrack_being(&mut self, being_unid: i64) {unsafe{
    let self_ptr: *mut Self = self as *mut _;
    let being_unid = &BeingUnid(being_unid);
    if let Some(this_being_loaded_tiles_set) = self.being_loaded_tiles_map.get(being_unid){
      for &tile_coord in this_being_loaded_tiles_set {
        (*self_ptr).decrement_shared_loads_count(tile_coord);
      }
      (*self_ptr).being_loaded_tiles_map.remove(being_unid);
    }
    else {godot_error!("{being_unid} specified to untrack not found");}
  }}

  pub fn tile_nid_mapping(&self) -> &Vec<TileDto> {&self.tile_unid_mapping}

// hacerlo async (no bloqueante)
//solo debería ejecutar esto el host y desp retransmitir los spawneos específicos
  #[func] fn do_natural_spawning(&mut self) {
          
  }

  #[signal] pub fn birth_being_kind(coords: Vector2i, id: StringName);
  #[signal] pub fn birth_being_w_init_data(coords: Vector2i, init_data: Dictionary);
  #[signal] pub fn being_unfrozen(coords: Vector2i, being_unid: i64);
}

fn exceeds_tile_limit(arr: &VariantArray) -> Result<(),()> {
  if arr.len() >= TileUnid::NULL.0 as usize {Ok(())} 
  else {Err(())}
}
