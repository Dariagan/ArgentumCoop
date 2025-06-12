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
use crate::utils::random_order::shuffle_and_into_iter;
use crate::utils::safe_vec::SafeVec;
use crate::utils::uns_vec::UnsVec;
use crate::utils::weighted_sample_hashmap;

use godot::builtin::Dictionary;
use godot::classes::{INode2D, Node2D, TileMapLayer};
use godot::prelude::*;
use std::borrow::{Borrow, BorrowMut};
use std::collections::{HashMap, HashSet};
#[derive(GodotClass)]
#[class(base=Node2D)]
pub struct RustTileMap {
  base: Base<Node2D>,
  #[var] seed: i64,
  #[var(get = get_tile_set_path)] tile_set_path: GString,
  #[var] beings_z_index: i32 /*z to assign on spawned beings*/,
  #[var] zlevel_layers: Array<Gd<TileMapLayer>>,
  tile_unid_mapping: Vec<TileDto>,
  tile_set: Gd<TileSet>,
  pub world_matrix: Option<WorldMatrix>,
  world_size: UnsVec,
  being_loaded_tiles_map: HashMap<BeingUnid, HashSet<UnsVec>> /*don't remove an entry directly*/,
  tile_shared_loads_count: HashMap<UnsVec, i64>  /*don't reduce this directly*/,
//-- beings section --

  now_loaded_gridpos: Array<Vector2i>,
  now_unloaded_gridpos: Array<Vector2i>,

  
  beings_in_chunk_count: Option<DownScalingMatrix<u16>>,
  pub spawn_weights_matrix: Option<SpawnWeightsMatrix>,
  rust_zlevel_layers: Vec<Gd<TileMapLayer>>,


}
#[godot_api] impl INode2D for RustTileMap {
  fn init(base: Base<Node2D>) -> Self {
    Self {
      base,
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

      now_loaded_gridpos: Array::new(),
      now_unloaded_gridpos: Array::new(),

      beings_in_chunk_count: None,
      spawn_weights_matrix: None,
      rust_zlevel_layers: Vec::with_capacity(TileZLevel::COUNT + 5),
    }
  }
  fn ready(&mut self) {
    self.base_mut().set_y_sort_enabled(true);
    for (i, layer_name) in TileZLevel::VARIANTS.iter().enumerate(){
      let mut new_child: Gd<TileMapLayer> = TileMapLayer::new_alloc(); let i: i32 = i as i32;
      new_child.set_name(*layer_name);
      self.base_mut().add_child(&new_child); self.base_mut().move_child(&new_child, i);
      new_child.set_z_index(i); new_child.set_rendering_quadrant_size(20); new_child.set_tile_set(&self.tile_set);
      self.zlevel_layers.push(&new_child);


      let z_level: TileZLevel = unsafe { std::mem::transmute(i as u8) };
      match z_level {
        TileZLevel::Stain | TileZLevel::Roof => new_child.set_navigation_enabled(false),
        TileZLevel::Structure => {
            new_child.set_y_sort_enabled(true); self.beings_z_index = i;
        }
        _ => (),
      }

      self.rust_zlevel_layers.push(new_child);
    }
  }
}
#[godot_api] 
impl RustTileMap {

  #[constant] const LAYER_COUNT: u16 = TileZLevel::COUNT as u16;    

  const TILE_SET_PATH: &'static str = "res://resource_instances/tiling/tset.tres";

  pub fn set_tile_state(&mut self, coords: UnsVec, tile_z_level: TileZLevel, state: Dictionary){
    let args: &[Variant; 3] = &[Into::<Vector2i>::into(coords).to_variant(), tile_z_level.to_variant(),state.to_variant()];
    self.base_mut().call("set_tile_state", args);
  }

  
  #[func]fn get_tile_set_path(&self) -> GString {self.tile_set_path.clone()}

  pub fn world_size(&self) -> UnsVec {self.world_size}

  #[func]
  fn generate_world_matrix(&mut self, size: Vector2i, tiles: Array<Gd<Tile>>) {
    
    assert!(tiles.len() < TileUnid::NULL.0 as usize);
    
    self.tile_unid_mapping.extend(tiles.iter_shared()
      .enumerate() 
      .map(|(i, mut tile)| {
        tile.bind_mut().unid = Some(TileUnid{0: i as u16}); tile.into()
      })
    );
    let size: UnsVec = size.try_into().expect("passed arg size: Vector2i is negative");

    const MIN_SIZE: u32 = 500;
    if size.all_bigger_than_min(MIN_SIZE).is_err(){
      godot_error!("world size is too small, must be at least {MIN_SIZE}X{MIN_SIZE}");
    }

    self.world_matrix = Some(WorldMatrix::new(size));     
    self.world_size = size;
    self.spawn_weights_matrix = Some(SpawnWeightsMatrix::new(size, Self::SWMAT_DOWNSCALE_FACTOR));
    self.beings_in_chunk_count = Some(DownScalingMatrix::new(size, Self::MACCHUNKMAT_DS_FACTOR));
  }
  #[func]
  fn generate_formation(&mut self, formation: FormGenEnum, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) -> bool{
    
    let now = std::time::Instant::now();

    generate(self, formation, origin, size, tile_selection, seed, data);
    godot_print!("time taken to generate: {:.2?}", now.elapsed());
    true
  }

  #[func]
  fn load_tiles_around(&mut self, being_coords: Vector2i, chunk_size: Vector2i, being_unid: i64) {
    let now = std::time::Instant::now();

    let chunk_size: SafeVec = SafeVec::from(chunk_size).all_bigger_than_min(10).expect("chunk size is smaller than minimum 10");
    let being_unid: BeingUnid = BeingUnid(being_unid);
    
    let being_coords: SafeVec = being_coords.into();
    let world_size: UnsVec = self.world_size;

    //self.now_loaded_gridpos.clear();

    for matrix_coord in (-chunk_size.lef as i32/2..chunk_size.lef as i32/2).flat_map(|i| (-chunk_size.right as i32/2..chunk_size.right as i32/2).map(move |j| (i,j)))
      .map(|vec| SafeVec::from(vec) + being_coords)
      .filter(|vec| vec.is_non_negative())
      .map(|vec|unsafe{UnsVec::try_from(vec).unwrap_unchecked()})
      .filter(|vec| vec.is_strictly_smaller_than(world_size)){unsafe{
        if ! self.tile_shared_loads_count.contains_key(&matrix_coord) {
          self.tile_shared_loads_count.insert(matrix_coord, 1);
          let tiles: TileUnidArray = self.world_matrix.as_ref().unwrap()[matrix_coord];
          

          tiles.iter().filter(|&&t_unid| t_unid != TileUnid::NULL).for_each(|&t_unid| {
            let (z_level, atlas, atlas_pos, alt_id) = self.get_cell_data(t_unid, matrix_coord);

            self.rust_zlevel_layers.get_unchecked_mut(z_level as usize).set_cell_ex(Into::<Vector2i>::into(matrix_coord))
              .source_id(atlas).atlas_coords(atlas_pos).alternative_tile(alt_id).done();

            }
          );
          //self.now_loaded_gridpos.push(Into::<Vector2i>::into(matrix_coord));
        } 
        else if !self.being_loaded_tiles_map.get(&being_unid).map_or(false, |set| set.contains(&matrix_coord)) {
          *self.tile_shared_loads_count.get_mut(&matrix_coord).unwrap() += 1;
        }
        self.being_loaded_tiles_map.entry(being_unid).or_insert_with(|| HashSet::with_capacity(chunk_size.area()*3/2)).insert(matrix_coord);
          
      }}

    godot_print!("load: {:.2?}", now.elapsed());
    let now = std::time::Instant::now();
    //self.now_unloaded_gridpos.clear();
    self.unload_excess_tiles(being_coords, chunk_size.into(), being_unid);
    godot_print!("unload: {:.2?}", now.elapsed());
  }
  fn get_cell_data(&mut self, unid: TileUnid, matrix_coord: UnsVec) -> (i32, i32, Vector2i, i32) {
    let tile: &TileDto = self.tile_nid_mapping().get(unid.0 as usize).expect(format!("tile mapped to {unid} not found").as_str());
    let tile_z_level: i32 = tile.z_level as i32;
    let atlas_origin_position: Vector2i = (*tile).origin_position;
    let atlas_origin_position_offset: Vector2i = matrix_coord.mod_unsv((*tile).modulo_tiling_area).into(); 
    (tile_z_level, (*tile).source_atlas, atlas_origin_position+atlas_origin_position_offset, (*tile).alternative_id)
  }
  fn unload_excess_tiles(&mut self, being_coords: SafeVec, chunk_size: UnsVec, being_unid: BeingUnid) {unsafe {
    let self_ptr: *mut Self = self as *mut _;

    if let Some(loaded_tiles) = self.being_loaded_tiles_map.get_mut(&being_unid) {

      loaded_tiles.retain(|&tile_coord| {
        let keep: bool = chunk_size.within_bounds_centered(SafeVec::from(tile_coord) - being_coords);
        
        if ! keep {(*self_ptr).decrement_shared_loads_count(tile_coord);}
        keep
      });
    }
  }}
  fn decrement_shared_loads_count(&mut self, tile_coord: UnsVec) {
    if let Some(&mut mut count) = self.tile_shared_loads_count.get_mut(tile_coord.borrow()) {
      if count > 0 {count -= 1;}
      
      if count == 0 {
        self.tile_shared_loads_count.remove(tile_coord.borrow());
        // let tile_coord: &[Variant; 1] = &[Into::<Vector2i>::into(tile_coord).to_variant()];
        // self.base_mut().call("tile_unloaded", tile_coord);
        for i in 0..TileZLevel::COUNT{
          self.rust_zlevel_layers[i].erase_cell(tile_coord.into());
          if i == TileZLevel::Structure as usize {continue;}
          //CAUSA PROBLEMAS CON CÓMO SE VEN LOS ÁRBOLES EL SET_CELL POSTERIOR
        }
      }
    }
    else{
      for i in 0..TileZLevel::COUNT{
        self.rust_zlevel_layers[i].erase_cell(tile_coord.into());
        if i == TileZLevel::Structure as usize {continue;}
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

#[constant]const BEING_LIMIT_PER_MACCHUNK: u16 = 5;  
#[constant]const SWMAT_DOWNSCALE_FACTOR: u32 = 20;  
#[constant]const SPAWNWEIGHTS_PER_MACROCHUNK_SIDE: u32 = 5;
#[constant]const MACCHUNKMAT_DS_FACTOR: u32 = Self::SWMAT_DOWNSCALE_FACTOR * Self::SPAWNWEIGHTS_PER_MACROCHUNK_SIDE;  

// hacerlo async (no bloqueante)
//solo debería ejecutar esto el host y desp retransmitir los spawneos específicos

//MEJOR HACERLO SOLO EN EL AREA EN EL Q SE DESCUBRA

//CREO Q SERIA MUCHO MAS RAPIDO SI EL ARRAY ESTA EN GDSCRIPT Y SE HACE TODO ESTO EN GDDSCRIPT
  #[func] fn do_natural_spawning(&mut self) {unsafe{
    cache_being_gen_templates_soil_tiles_unids(&self);
    let self_ptr: *mut Self = self as *mut _;
    if let Some(beings_in_chunk_count) = self.beings_in_chunk_count.as_mut() {
      if let Some(spawn_weights_matrix) = self.spawn_weights_matrix.as_ref() {
        let mut used_vec: Vec<u32> = (0..Self::SPAWNWEIGHTS_PER_MACROCHUNK_SIDE).collect();
        let mut used_vec_inner: Vec<u32> = (0..Self::SPAWNWEIGHTS_PER_MACROCHUNK_SIDE).collect();

        let spawning_used_vec_offset_lef: Vec<u32> = (0..Self::SWMAT_DOWNSCALE_FACTOR).collect();
        let spawning_used_vec_offset_right: Vec<u32> = (0..Self::SWMAT_DOWNSCALE_FACTOR).collect();
        
        let mut being_gen_templ_ids: Array<StringName> = Array::new();
        let mut spawns_coords: Array<Vector2i> = Array::new();
        let mut facs_ids: Array<StringName> = Array::new();

        //println!("chunksize {}", beings_in_chunk_count.size());
        for chunk_i in 0..beings_in_chunk_count.size().lef {
        for chunk_j in 0..beings_in_chunk_count.size().right {
          let chunk_coords = UnsVec::new(chunk_i, chunk_j );
          //println!("chunkcoords {}", chunk_coords);
'outer_sw:for sw_i in shuffle_and_into_iter(&mut used_vec, None) {
          for sw_j in shuffle_and_into_iter(&mut used_vec_inner, None) {
            if beings_in_chunk_count[chunk_coords] < Self::BEING_LIMIT_PER_MACCHUNK {
              let sw_unsvec = UnsVec::new(sw_i, sw_j);
              //println!("swuns{}", sw_unsvec);
              let spawnweight_coords: UnsVec = chunk_coords*Self::SPAWNWEIGHTS_PER_MACROCHUNK_SIDE + sw_unsvec;
              //println!("swcoords{}", spawnweight_coords);

              let sw_mapping: &HashMap<BeingGenTemplIdAndFac, spawn_weights_matrix::SpawnWeight> = spawn_weights_matrix.get_unchk_no_downscale(spawnweight_coords);
              
              if let Some(BeingGenTemplIdAndFac { being_gen_templ_id, fac_id }) = weighted_sample_hashmap::sample_from_weighted_map(sw_mapping) {
                let being_gen_templ: Gd<RustBeingGenTemplate> = retrieve_being_gen_template_from_id(&being_gen_templ_id);

                for _ in 0..Self::SWMAT_DOWNSCALE_FACTOR*Self::SWMAT_DOWNSCALE_FACTOR {
                  let rand_offset: UnsVec = UnsVec::new_from_thread_rng(0, Self::SWMAT_DOWNSCALE_FACTOR);
                  let spawn_coords: UnsVec = chunk_coords*Self::MACCHUNKMAT_DS_FACTOR + sw_unsvec*Self::SWMAT_DOWNSCALE_FACTOR + rand_offset;
                  let tile_unid_arr: TileUnidArray = self.world_matrix.as_ref().unwrap_unchecked()[spawn_coords];
                  let soil_tile_unid: TileUnid = tile_unid_arr[TileZLevel::Soil]; let structure_tile_unid: TileUnid = tile_unid_arr[TileZLevel::Structure];

                  if structure_tile_unid != TileUnid::NULL {continue;}
                  if let Some(allowed_soil_tiles_unids) = being_gen_templ.bind().allowed_soil_tiles_unids(){
                    if allowed_soil_tiles_unids.contains(&soil_tile_unid) {
                      let spawn_coords: Vector2i = spawn_coords.into();
                      being_gen_templ_ids.push(&being_gen_templ_id); spawns_coords.push(spawn_coords); facs_ids.push(&fac_id);
                      beings_in_chunk_count[chunk_coords] += 1;
                      break;
                    }
                  }
                }
              } 
            } 
            else {break 'outer_sw;}
          }}
        }}
        let varargs: &[Variant; 3] = &[being_gen_templ_ids.to_variant(), spawns_coords.to_variant(), facs_ids.to_variant()];
        (*self_ptr).base_mut().call("mass_birth_being_gen_template_at_snapped", varargs);


      } else {godot_error!("Error: spawn_weights_matrix is None");}
    } else {godot_error!("Error: beings_in_chunk_count is None");}
  }}

  #[signal] pub fn birth_from_init_data(coords: Vector2i, init_data: Dictionary);
  #[signal] pub fn being_unfrozen(coords: Vector2i, being_unid: i64);
  #[signal] pub fn instantiate_faction(faction_defining_data: Dictionary);

}

fn exceeds_tile_limit(arr:&VariantArray)->Result<(),()>{if arr.len()>=TileUnid::NULL.0 as usize {Ok(())}else{Err(())}}
