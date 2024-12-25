pub(crate) mod world_matrix;

use godot::classes::TileSet;
use spawn_weights_matrix::SpawnWeightsMatrix;
use strum::EnumCount;
use strum::VariantNames;
use world_matrix::*;
use crate::beings::*;
use crate::formation_generation::*;
use crate::tiling::TileDto;
use crate::utils::matrix::DownScalingMatrix;
use crate::utils::safe_vec::SafeVec;
use crate::utils::uns_vec::UnsVec;
use godot::builtin::Dictionary;
use godot::classes::{INode2D, Node2D, TileMapLayer};
use godot::prelude::*;
use std::borrow::{Borrow, BorrowMut};
use std::collections::{HashMap, HashSet};
#[derive(GodotClass)]#[class(base=Node2D)]
struct RustTileMap {
  base: Base<Node2D>,
  #[var] layer_count: u16,
  #[var] seed: i64,
  #[var(get = get_tile_set_path)] tile_set_path: GString,
  #[var] beings_z_index: i32 /*z to assign on spawned beings*/,
  #[var] zlevel_layers: Array<Gd<TileMapLayer>>,
  tile_unid_mapping: Vec<TileDto>,
  tile_set: Gd<TileSet>,
  world_matrix: Option<WorldMatrix>,
  world_size: UnsVec,
  being_loaded_tiles_map: HashMap<BeingUnid, HashSet<UnsVec>> /*don't remove an entry directly*/,
  tile_shared_loads_count: HashMap<UnsVec, i64>  /*don't reduce this directly*/,
//-- beings section --
  beings_in_chunk_count: Option<DownScalingMatrix<u16>>,
  spawn_weights_matrix: Option<SpawnWeightsMatrix>,

}

#[godot_api] impl INode2D for RustTileMap {
  fn init(base: Base<Node2D>) -> Self {
    Self {
      base,
      layer_count: TileZLevel::COUNT as u16,
      seed: 0,
      tile_set_path: RustTileMap::TILE_SET_PATH.into(),
      tile_set: try_load(RustTileMap::TILE_SET_PATH).expect(format!("couldn't find tile set in {}", RustTileMap::TILE_SET_PATH).as_str()),
      world_matrix: None,
      tile_unid_mapping: Vec::new(),
      world_size: UnsVec::ZERO,
      zlevel_layers: Default::default(),
      beings_z_index: -1,
      being_loaded_tiles_map: Default::default(),
      tile_shared_loads_count: Default::default(),
      spawn_weights_matrix: None,
      beings_in_chunk_count: None
    }
  }
  fn ready(&mut self) {
    self.base_mut().set_y_sort_enabled(true);

    let layer_names: Array<StringName> = TileZLevel::VARIANTS
      .iter().map(|&name| StringName::from(name)).collect();

    for (i, layer_name) in layer_names.iter_shared().enumerate(){
      let mut new_child: Gd<TileMapLayer> = TileMapLayer::new_alloc(); let i: i32 = i as i32;
      new_child.set_name(&layer_name.to_string());
      self.base_mut().add_child(&new_child);
      self.base_mut().move_child(&new_child, i);
      new_child.set_z_index(i); new_child.set_rendering_quadrant_size(20); 
      new_child.set_tile_set(&self.tile_set);

      self.zlevel_layers.push(&new_child);

      if layer_name.to_string() == "Structure" {
          self.beings_z_index = i; new_child.set_y_sort_enabled(true);
      }
    }
  }
}
#[godot_api] impl RustTileMap {
  #[constant] const MACROSCOPIC_SPAWNING_CHUNK_SIZE: u8 = 15;
  #[constant] const BEING_LIMIT_PER_MACROSCOPIC_SPAWNING_CHUNK: u16 = 200;  
  const TILE_SET_PATH: &'static str = "res://resource_instances/tiling/tset.tres";
  
  #[func]fn get_tile_set_path(&self) -> GString {return self.tile_set_path.clone();}
  
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

    let chunk_size: SafeVec = SafeVec::from(chunk_size).all_bigger_than_min(10).expect("chunk size is smaller than minimum 10");
    let being_unid: BeingUnid = BeingUnid(being_unid);
    
    let being_coords: SafeVec = _being_coords.into();
    let world_size: UnsVec = self.world_size;

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
      self.base_mut().get_child(tile_z_level).unwrap_unchecked().call("set_cell", &args);
    }
  }
  fn unload_excess_tiles(&mut self, being_coords: SafeVec, chunk_size: UnsVec, being_unid: BeingUnid) {unsafe {
    let self_ptr: *mut Self = self as *mut _;

    let loaded_tiles: &mut HashSet<UnsVec> = self.being_loaded_tiles_map.get_mut(&being_unid).unwrap_unchecked();

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
          unsafe{self.base_mut().get_child(layer_i as i32).unwrap_unchecked().call("erase_cell", tile_coord);}
        }
        self.base_mut().emit_signal("tile_unloaded", tile_coord);
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
