
use std::borrow::BorrowMut;
use rust_tilemap::RustTileMap;

use crate::beings::{spawn_weights_matrix::{self, SpawnWeight, SpawnWeightsMatrix}, BeingGenTemplIdAndFac};

use super::*;
use std::collections::{HashMap, HashSet};


#[derive(strum_macros::VariantNames, EnumCount)] #[allow(non_camel_case_types, dead_code)]
//enum Target{floor = 0, wall, lake }//no poner targets nuevos al final (ahí van las caves)
enum Target{beach = 0, oceanfloor, lake, cont, tree, bush, ocean, cave0, cave1, cave2}

struct DungeonGenerator{}

impl IFormationGenerator for DungeonGenerator {
fn generate(tilemap: &mut RustTileMap, origin: UnsVec, size: UnsVec, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary,){

let world: &mut WorldMatrix = &mut tilemap.world_matrix.as_mut().expect("world matrix needs to be generated before formation (call generate_world_matrix first)");

let sw_mat: &mut SpawnWeightsMatrix = &mut tilemap.spawn_weights_matrix.as_mut().expect("spawn weights matrix needs to be generated before formation (call generate_world_matrix first)");
 
let mut unidordist_mapped2targets: [UnidOrDist; Target::COUNT] = Default::default();
crate::tiling::fill_targets(&mut unidordist_mapped2targets, Target::VARIANTS, tile_selection);

 


}

}
//TODO chequear si se va fuera del mapa. usar modulo
// pub fn random_walk(start_pos: UnsVec, rng: &mut Lcg128Xsl64, walk_length: usize) -> Vec<UnsVec> {
//     let mut walk: Vec<UnsVec> = Vec::with_capacity(walk_length);
//     let mut pos = start_pos;
//     walk.push(pos);
//     for _ in 0..walk_length {
//         let dir = Direction::random();
//         pos += dir;
//         walk.push(pos);
//     }
//     walk
// }