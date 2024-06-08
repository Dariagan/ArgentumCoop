pub use crate::tiling::TileSelection;
use crate::uns_vec::UnsVec;
use crate::world_matrix::*;
pub use crate::{safe_vec::SafeVec, world_matrix::WorldMatrix};
pub use godot::builtin::Dictionary;
use enum_primitive_derive::Primitive;
use godot::obj::{Base, Gd, GdRef};
use godot::prelude::*;

#[derive(GodotConvert, Var, Export, Primitive)]
#[godot(via = i64)]
pub enum FormGenEnum {
    FracturedFormationGenerator = 0,
}


pub trait IFormationGenerator {
    fn generate(
        world: WorldMatrix,
        origin: UnsVec,
        size: UnsVec,
        tile_selection: Gd<TileSelection>,
        seed: i64,
        data: Dictionary,
    ) -> WorldMatrix;
}

pub fn get_border_closeness_factor(
    coords: &UnsVec,
    world_size: &UnsVec,
    power: Option<f64>,
) -> f64 {
    let power = power.unwrap_or(3.0);

    let horizontal_border_closeness: f64 = ((coords.lef as f64 - world_size.lef as f64 / 2.0)
        / (world_size.lef as f64 / 2.0))
        .abs()
        .powf(power);
    let vertical_border_closeness: f64 = ((coords.right as f64 - world_size.right as f64 / 2.0)
        / (world_size.right as f64 / 2.0))
        .abs()
        .powf(power);

    horizontal_border_closeness.max(vertical_border_closeness)
}

pub fn overwrite_formation_tile(world_matrix: *mut WorldMatrix, (origin, relative): (UnsVec, UnsVec), nid_or_dist: NidOrDist, instantiation_data: Option<Dictionary>){
    unsafe{
        // z level de tiles falta, volver nidorniddistribution Gd<tile>distribution?
                 
        match nid_or_dist {
            //EL ESTADO HAY Q ASIGNARLO EN OTRO LADO, SINO CADA CASILLA VA A OCUPAR MUCHO MÁS DE 2 BYTES Y EL USO DE MEMORIA SE VUELVE EXCESIVO.
            //PARA EL ESTADO USAR UN DICT <COORD,STATE> EN GODOT, Q SE ASIGNE DESDE ACA CON UNA SIGNAL
            NidOrDist::Nid((nid, z_level)) => {(*world_matrix).overwrite_tile(nid, origin+relative, z_level);},
            NidOrDist::Dist(dist) => {
                let (nid,z_level) = dist.sample();
                (*world_matrix).overwrite_tile(nid, origin+relative, z_level);
            }

        }
        return;
        
        todo!()
    }
}

pub fn generate_stateful_instance(world_matrix: *mut WorldMatrix, (origin, relative): (UnsVec, UnsVec), nid: NidOrDist, instantiation_data: Dictionary){
    todo!()
    //llamar señal o algo
}

