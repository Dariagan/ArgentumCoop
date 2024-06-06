use godot::{builtin::Dictionary, obj::Gd};
use strum_macros::{EnumString}; use strum::{EnumCount, VariantNames}; use strum_macros::{EnumCount as EnumCountMacro};
use crate::{formation_generator::{IFormationGenerator, TileSelection}, uns_vec::UnsVec, world_matrix::{TileTypeNid, WorldMatrix}};

pub struct FracturedFormationGenerator{}

#[derive(strum_macros::VariantNames, EnumCount)]
#[strum(serialize_all = "snake_case")]
enum Target{ Beach = 0, Lake, Cont, Tree, Bush, Ocean, Cave0, Cave1, Cave2}



impl IFormationGenerator for FracturedFormationGenerator {
    fn generate(world: WorldMatrix, origin: UnsVec, size: UnsVec,
        tile_selection_set: Gd<TileSelection>, seed: i64, data: Dictionary,
    ) -> WorldMatrix{

        let nids_mapped_to_targets: [TileTypeNid; Target::COUNT];

        todo!()

    }
    
}
