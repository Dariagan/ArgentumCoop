mod fractured_continent_generator;

pub use godot::builtin::Dictionary;
pub use rust_tilemap::world_matrix::*;

use crate::rust_tilemap;
use crate::utils::raw_pointers::*;
use crate::utils::uns_vec::UnsVec;
use fastnoise_lite::*;
use std::thread::{self, JoinHandle};
use rand_seeder::Seeder;
use rand_pcg::Pcg64;
use rand_pcg::Lcg128Xsl64;
use rand::Rng;
use strum::{EnumCount, VariantNames};
use enum_primitive_derive::Primitive;
use godot::obj::{Base, Gd, GdRef};
use godot::prelude::*;
use gxhash::HashSet;

use strum::IntoEnumIterator;

use fractured_continent_generator::*;
#[derive(GodotConvert, Var, Export, Primitive, Debug, strum_macros::VariantNames, EnumCount, Display)]
#[strum(serialize_all = "PascalCase")]
#[godot(via = i64)]
pub enum FormGenEnum {
  FracturedContinentGenerator = 0,
}

pub fn generate(world_matrix: WorldMatrix, formation: FormGenEnum, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) -> WorldMatrix{

  let (origin, size) = 
      (UnsVec::try_from(origin).expect("({} at {}, {}): passed arg origin: Vector2i is negative"), 
      UnsVec::try_from(size).expect("({} at {}, {}): passed arg size: Vector2i is negative")); 

  const MIN_SIZE: u32 = 100;
  if size.all_bigger_than_min(MIN_SIZE).is_err(){
    panic!("({formation} origin:{origin} size:{size}): formation size is too small, must be at least {MIN_SIZE}X{MIN_SIZE}");
  }
  if origin.lef + size.lef > world_matrix.size().lef{
    panic!("({formation} origin:{origin} size:{size}): formation would go out of world bounds eastward");
  }
  if origin.right + size.right > world_matrix.size().right{
    panic!("({formation} origin:{origin} size:{size}): formation would go out of world bounds southward");
  }
  match formation {
    FormGenEnum::FracturedContinentGenerator => {
      FracturedContinentGenerator::generate(world_matrix, origin, size, tile_selection, seed, data)
    }
  }
}

pub trait IFormationGenerator {
  fn generate(
    world: WorldMatrix,
    origin: UnsVec,
    size: UnsVec,
    tile_selection: Gd<TileSelection>,
    seed: i32,
    data: Dictionary,
  ) -> WorldMatrix;
}
pub fn overwrite_all_tiles_at_coord(mut world: SendMutPtr<WorldMatrix>, (origin, relative): (UnsVec, UnsVec), unids: &TileUnidArray, instantiation_data: Option<Dictionary>){
  unsafe{
    for (&unid, z_level) in unids.iter().zip(TileZLevel::iter()) {
      
      world.drf().overwrite_tile(unid, origin+relative, z_level);
    }
  }
}

pub fn overwrite_formation_tile(mut world: SendMutPtr<WorldMatrix>, (origin, relative): (UnsVec, UnsVec), (unid, z_level) : (TileUnid, TileZLevel), instantiation_data: Option<Dictionary>){
  unsafe{
    world.drf().overwrite_tile(unid, origin+relative, z_level);
  }
}

pub fn generate_stateful_instance(world_matrix: *mut WorldMatrix, (origin, relative): (UnsVec, UnsVec), unid: UnidOrDist, instantiation_data: Dictionary){
  todo!("no termine generate statful")
  //llamar señal o algo
}


#[inline]
pub fn calc_continentness(continenter: SharedNoise, rel_coords: UnsVec, size: UnsVec, power: Option<f32>, offset: Option<UnsVec>) -> f32 {
  let bff = get_border_farness_factor(rel_coords, size, power);
  let val = bff * continenter.get_noise_2d(rel_coords + offset.unwrap_or_default());
  val
}
#[inline]
pub fn noise_surpasses_cutoff(fast_noise_lite: SharedNoise, rel_coords: UnsVec, cutoff: f32) ->(f32, bool){
  let nv = fast_noise_lite.get_noise_2d(rel_coords); (nv, nv > cutoff)
}
#[inline]
pub fn val_surpasses_cutoff(val: f32, cutoff: f32) ->(f32, bool){(val, val > cutoff)}

pub fn get_border_farness_factor(rel_coords: UnsVec, world_size: UnsVec, power: Option<f32>) -> f32 {
  let power = power.unwrap_or(10.0);

  let horizontal_border_closeness: f32 = ((rel_coords.lef as f32 - world_size.lef as f32/2.0)/(world_size.lef as f32/2.0)).abs().powf(power);
  let vertical_border_closeness: f32 = ((rel_coords.right as f32 - world_size.right as f32/2.0)/(world_size.right as f32/2.0)).abs().powf(power);
  1.0 - horizontal_border_closeness.max(vertical_border_closeness)
}
pub fn clear_of(set_to_check: &HashSet<UnsVec>, rel_coords: UnsVec, radius: i32, check_forwards: bool,) -> bool {
  for lef in -(radius)..=(check_forwards as i32 * radius) {
    for right in -(radius)..=radius{
      if set_to_check.contains(&UnsVec::new((rel_coords.lef as i32 + lef) as u32,( rel_coords.right as i32 + right) as u32)) {
        return false;
      }
    }
  }
  true
}
#[derive(Clone, Copy)]
pub struct SharedNoise{pub ns_ptr: SendPtr<FastNoiseLite>}
impl SharedNoise {
  #[inline]
  pub fn new(fast_noise_lite: &FastNoiseLite) -> Self {Self { ns_ptr: make_ptr!(fast_noise_lite) }}
  #[inline]
  pub fn get_noise_2d(&self, rel_coords: UnsVec) -> f32 {
    unsafe{(&*self.ns_ptr.0).get_noise_2d(rel_coords.lef as f64, rel_coords.right as f64) + 1.0}
  }
}