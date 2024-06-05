use godot::{builtin::Dictionary, obj::Gd};

use crate::{formation_generator::{IFormationGenerator, TileSelection}, uns_vec::UnsVec, world_matrix::WorldMatrix};

pub struct FracturedFormationGenerator{}


impl IFormationGenerator for FracturedFormationGenerator {
    fn generate(world: WorldMatrix, origin: UnsVec, size: UnsVec,
        tile_selection_set: Gd<TileSelection>, seed: i64, data: Dictionary,
    ) -> WorldMatrix{
        todo!()
    }
    
}
