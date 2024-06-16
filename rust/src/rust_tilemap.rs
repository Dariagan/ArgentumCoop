
use crate::formation_generator::{FormGenEnum, IFormationGenerator, };
use crate::fractured_continent_generator::FracturedContinentGenerator;
use crate::safe_vec::SafeVec;
use crate::uns_vec::{UnsVec, ZERO};
use crate::world_matrix::*;
use godot::builtin::Dictionary;
use godot::engine::{ITileMap, TileMap};
use godot::prelude::*;
use gxhash::{HashMap, HashSet, HashSetExt as _};
use std::borrow::Borrow;
use std::fmt;
use std::hash::{Hash, Hasher};

#[derive(GodotClass)]
#[class(base=TileMap)]
struct RustTileMap {
  #[var]
  seed: i64,
  tile_nid_mapping: Vec<TileDto>,
  world_matrix: Option<WorldMatrix>,
  base: Base<TileMap>,
  world_size: UnsVec,
  //don't remove an entry directly
  being_loaded_tiles_map: HashMap<BeingUnid, HashSet<UnsVec>>,
  //don't reduce this directly
  tile_shared_loads_count: HashMap<UnsVec, i64>,
}
#[godot_api]
impl ITileMap for RustTileMap {
  fn init(base: Base<TileMap>) -> Self {
    Self {
      base,
      seed: 0,
      world_matrix: None,
      tile_nid_mapping: Vec::new(),
      world_size: ZERO,
      being_loaded_tiles_map: Default::default(),
      tile_shared_loads_count: Default::default()
    }
  }
}
#[godot_api]
impl RustTileMap {
  #[func]
  fn generate_world_matrix(&mut self, size: Vector2i, tiles: Array<Gd<Tile>>) {
    
    assert!(tiles.len() < NULL_TILE.0 as usize);
    
    self.tile_nid_mapping.extend(tiles.iter_shared()
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
    self.world_size = size
  }
  #[func]
  fn generate_formation(&mut self, formation: FormGenEnum, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) -> bool{
    let (origin, size) = 
      (UnsVec::try_from(origin).expect("passed arg origin: Vector2i is negative"), 
      UnsVec::try_from(size).expect("passed arg size: Vector2i is negative")); 

    const MIN_SIZE: u32 = 100;
    if size.all_bigger_than_min(MIN_SIZE).is_err(){
      godot_error!("formation size is too small, must be at least {MIN_SIZE}X{MIN_SIZE}");
      return false;
    }
    if origin.lef + size.lef > self.world_matrix.as_ref().expect("world matrix was not generated before formation").size().lef{
      godot_error!("formation would go out of world bounds eastward");
      return false;
    }
    unsafe{
      if origin.right + size.right > self.world_matrix.as_ref().unwrap_unchecked().size().right{
        godot_error!("formation would go out of world bounds southward");
        return false;
      }
    }
    use std::time::Instant;
    let now = Instant::now();
    self.world_matrix = Some(
      match formation {
        FormGenEnum::FracturedContinentGenerator => {
          FracturedContinentGenerator::generate(
          self.world_matrix.take().expect("world matrix not present when attempting to generate"), origin, size, tile_selection, seed, data)
      }
    });
    let elapsed = now.elapsed();
    godot_print!("rust time: {:.2?}", elapsed);
    true
  }

  #[func]
  fn load_tiles_around(&mut self, _local_coords: Vector2, chunk_size: Vector2i, being_unid: i64) {

    let chunk_size = SafeVec::from(chunk_size).all_bigger_than_min(10).expect("chunk size is smaller than minimum 10");
    let being_unid = BeingUnid(being_unid);
    
    let being_coords: SafeVec = self.base().local_to_map(_local_coords).into(); let _local_coords = ();

    let world_size = self.world_size;

    for chunk_coord in (-chunk_size.lef as i32/2..chunk_size.lef as i32/2).flat_map(|i| (-chunk_size.right as i32/2..chunk_size.right as i32/2).map(move |j| (i,j)))
      .map(|vec| SafeVec::from(vec) + being_coords)
      .filter(|vec| vec.is_non_negative())
      .map(|vec|unsafe{UnsVec::try_from(vec).unwrap_unchecked()})
      .filter(|vec| vec.is_strictly_smaller_than(world_size)){unsafe{
        if ! self.tile_shared_loads_count.contains_key(&chunk_coord) {
          
          self.tile_shared_loads_count.insert(chunk_coord, 1);

          let tiles = self.world_matrix.as_ref().unwrap_unchecked()[chunk_coord];
          tiles.iter().filter(|&&unid| unid != NULL_TILE).for_each(|&unid| self.set_cell(unid, chunk_coord));
        } 
        else if !self.being_loaded_tiles_map.get(&being_unid).map_or(false, |set| set.contains(&chunk_coord)) {
            *self.tile_shared_loads_count.get_mut(&chunk_coord).unwrap_unchecked() += 1;
        }
        self.being_loaded_tiles_map.entry(being_unid).or_insert_with(|| HashSet::with_capacity(chunk_size.area()*3/2)).insert(chunk_coord);
          
      }}
    self.unload_excess_tiles(being_coords, chunk_size.into(), being_unid);
  }
  //todo fix hay algo mal

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
  
  fn unload_excess_tiles(&mut self, being_coords: SafeVec, chunk_size: UnsVec, being_unid: BeingUnid) {
    unsafe {
      let self_ptr: *mut Self = self as *mut _;

      let loaded_tiles = self.being_loaded_tiles_map.get_mut(&being_unid).unwrap_unchecked();

      loaded_tiles.retain(|&tile_coord| {
          let keep: bool = chunk_size.within_bounds_centered(SafeVec::from(tile_coord) - being_coords);
          
          if keep == false {(*self_ptr).decrement_shared_loads_count(tile_coord);}
          keep
      });
    }
  }
  #[signal]
  pub fn tile_unloaded(coords: Vector2i);

  fn decrement_shared_loads_count(&mut self, tile_coord: UnsVec) {
    if let Some(count) = self.tile_shared_loads_count.get_mut(tile_coord.borrow()) {
      if *count > 0 {*count -= 1;}
      
      if *count == 0 {
          for layer_i in 0..self.base().get_layers_count() {
              self.base_mut().erase_cell(layer_i, tile_coord.into());
          }
          self.tile_shared_loads_count.remove(tile_coord.borrow());
          let tile_coord: Vector2i = tile_coord.into();
          let tile_coord: [Variant; 1] = [tile_coord.to_variant()];
          self.base_mut().emit_signal("tile_unloaded".into(), &tile_coord);
      }
    }
  }

  fn set_cell(&mut self, unid: TileUnid, matrix_coord: UnsVec) {
    let tile: *const TileDto = self.tile_nid_mapping().get(unid.0 as usize).expect("tile mapped to unid= not found");
    unsafe{
    let atlas_origin_position = (*tile).origin_position;
    let modulo_tiling_area = (*tile).modulo_tiling_area;
    let atlas_origin_position_offset = matrix_coord.mod_unsv(modulo_tiling_area.try_into().unwrap_unchecked()); 
    self.base_mut().set_cell_ex((*tile).z_level as i32, matrix_coord.into())
      .source_id((*tile).source_atlas)
      .atlas_coords(atlas_origin_position + atlas_origin_position_offset.into())
      .alternative_tile((*tile).alternative_id)
      .done()
  }}

  pub fn tile_nid_mapping(&self) -> &Vec<TileDto> {&self.tile_nid_mapping}
}

fn exceeds_tile_limit(arr: &VariantArray) -> Result<(),()> {
  if arr.len() >= NULL_TILE.0 as usize {Ok(())} 
  else {Err(())}
}

#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
struct BeingUnid(i64);
impl Hash for BeingUnid {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i64(self.0);}}
impl fmt::Display for BeingUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bunid{}", self.0)}}