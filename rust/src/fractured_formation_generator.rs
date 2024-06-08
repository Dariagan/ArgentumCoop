use godot::{builtin::Dictionary, obj::Gd};
use strum_macros::{EnumString}; use strum::{EnumCount, VariantNames}; use strum_macros::{EnumCount as EnumCountMacro};
use crate::{formation_generator::{IFormationGenerator, TileSelection}, uns_vec::UnsVec, world_matrix::{NidOrDist, WorldMatrix}};

pub struct FracturedFormationGenerator{}

#[derive(strum_macros::VariantNames, EnumCount)]
#[strum(serialize_all = "snake_case")]
enum Target{ Beach = 0, Lake, Cont, Tree, Bush, Ocean, Cave0, Cave1, Cave2}



impl IFormationGenerator for FracturedFormationGenerator {
    fn generate(world: WorldMatrix, origin: UnsVec, size: UnsVec,
        tile_selection: Gd<TileSelection>, seed: i64, data: Dictionary,
    ) -> WorldMatrix{

        let mut nids_mapped_to_targets: [NidOrDist; Target::COUNT] = Default::default();
        crate::tiling::fill_targets(&mut nids_mapped_to_targets, Target::VARIANTS, tile_selection);
        todo!()

    }
}
