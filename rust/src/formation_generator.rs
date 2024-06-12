use std::thread::current;

use crate::safe_vec::SafeVec;
pub use crate::tiling::TileSelection;
use crate::uns_vec::UnsVec;
use crate::utils::*;
use crate::world_matrix::*;
pub use crate::{world_matrix::WorldMatrix};
use fastnoise_lite::FastNoiseLite;
pub use godot::builtin::Dictionary;
use enum_primitive_derive::Primitive;
use godot::obj::{Base, Gd, GdRef};
use godot::prelude::*;

#[derive(GodotConvert, Var, Export, Primitive, Debug)]
#[godot(via = i64)]
pub enum FormGenEnum {
    FracturedContinentGenerator = 0,
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

pub fn overwrite_formation_tile(mut world: SendMutPtr<WorldMatrix>, (origin, relative): (UnsVec, UnsVec), (unid, z_level) : (TileUnid, TileZLevel), instantiation_data: Option<Dictionary>){
    unsafe{
        world.drf().overwrite_tile(unid, origin+relative, z_level);
        
        return;
        
    }
}

pub fn generate_stateful_instance(world_matrix: *mut WorldMatrix, (origin, relative): (UnsVec, UnsVec), unid: NidOrDist, instantiation_data: Dictionary){
    todo!("no termine generate statful")
    //llamar señal o algo
}

#[inline]
pub fn is_continental(continenter: SendPtr<FastNoiseLite>, rel_coords: UnsVec, size: UnsVec, continenter_cutoff: f32, power: Option<f32>, offset: Option<UnsVec>) -> bool {
    get_continentness(continenter, rel_coords, size, power, offset) > continenter_cutoff
}
#[inline]
pub fn get_continentness(continenter: SendPtr<FastNoiseLite>, rel_coords: UnsVec, size: UnsVec, power: Option<f32>, offset: Option<UnsVec>) -> f32 {
    let bff = crate::formation_generator::get_border_farness_factor(rel_coords, size, power);
    let val = bff *get_noise_value(continenter, rel_coords + offset.unwrap_or_default());
    val
}
#[inline]
pub fn nv_surpasses_cutoff(fast_noise_lite: SendPtr<FastNoiseLite>, rel_coords: UnsVec, cutoff: f32) -> bool{
    get_noise_value(fast_noise_lite, rel_coords) > cutoff
}
#[inline]
pub fn get_noise_value(fast_noise_lite: SendPtr<FastNoiseLite>, rel_coords: UnsVec) -> f32 {
    unsafe{(&*fast_noise_lite.0).get_noise_2d(rel_coords.lef as f64, rel_coords.right as f64)}
}
pub fn get_border_farness_factor(
    rel_coords: UnsVec,
    world_size: UnsVec,
    power: Option<f32>,
) -> f32 {
    let power = power.unwrap_or(3.0);

    let horizontal_border_closeness: f32 = ((rel_coords.lef as f32 - world_size.lef as f32 / 2.0)
        / (world_size.lef as f32 / 2.0))
        .abs()
        .powf(power);
    let vertical_border_closeness: f32 = ((rel_coords.right as f32 - world_size.right as f32 / 2.0)
        / (world_size.right as f32 / 2.0))
        .abs()
        .powf(power);
    1.0 - horizontal_border_closeness.max(vertical_border_closeness)
}

