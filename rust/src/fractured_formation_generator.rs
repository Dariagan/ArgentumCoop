use std::thread::{self, JoinHandle};

use godot::{builtin::Dictionary, classes::*, obj::*};
use godot::classes::fast_noise_lite::NoiseType;

use godot::prelude::*;
use strum_macros::{EnumString}; use strum::{EnumCount, VariantNames}; use strum_macros::{EnumCount as EnumCountMacro};
use crate::world_matrix::{TileTypeNid, TileZLevel};
use crate::{formation_generator::{IFormationGenerator, TileSelection}, uns_vec::UnsVec, world_matrix::{NidOrDist, WorldMatrix}};

pub struct FracturedFormationGenerator{}

#[derive(strum_macros::VariantNames, EnumCount)]
#[strum(serialize_all = "snake_case")]
enum Target{ Beach = 0, Lake, Cont, Tree, Bush, Ocean, Cave0, Cave1, Cave2}



impl IFormationGenerator for FracturedFormationGenerator {
    fn generate(mut world: WorldMatrix, origin: UnsVec, size: UnsVec,
        tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary,
    ) -> WorldMatrix{

        let mut nids_mapped_to_targets: [NidOrDist; Target::COUNT] = Default::default();
        crate::tiling::fill_targets(&mut nids_mapped_to_targets, Target::VARIANTS, tile_selection);
        
        let mut continenter = FastNoiseLite::new_gd(); continenter.set_noise_type(NoiseType::VALUE_CUBIC);
        let mut peninsuler = FastNoiseLite::new_gd(); peninsuler.set_noise_type(NoiseType::VALUE_CUBIC);
        let mut big_laker = FastNoiseLite::new_gd(); big_laker.set_noise_type(NoiseType::VALUE_CUBIC);
        let mut small_laker = FastNoiseLite::new_gd(); continenter.set_noise_type(NoiseType::VALUE_CUBIC);
        let mut continenter = FastNoiseLite::new_gd(); continenter.set_noise_type(NoiseType::VALUE_CUBIC);
        let mut continenter = FastNoiseLite::new_gd(); continenter.set_noise_type(NoiseType::VALUE_CUBIC);
        let mut continenter = FastNoiseLite::new_gd(); continenter.set_noise_type(NoiseType::VALUE_CUBIC);
        let mut continenter = FastNoiseLite::new_gd(); continenter.set_noise_type(NoiseType::VALUE_CUBIC);

        let continental_cutoff = 0.61 * f64::powf(size.length()/1600.0, 0.05);
        continenter.set_seed(seed);
        
        const N_THREADS: usize = 16;
        let mut threads: [Option<JoinHandle<()>>; N_THREADS] = Default::default();

        for thread_i in 0..N_THREADS {threads[thread_i] = Some(thread::spawn(move || {
            let hori_range = ((thread_i*size.lef as usize/N_THREADS) as u32, (thread_i*size.lef as usize/N_THREADS) as u32);
            for coords in (hori_range.0..hori_range.1).zip(0..size.lef).map(UnsVec::from) {

            }
        }))}        
        for thread in threads {
            if let Some(thread) = thread{
                let _ = thread.join();
            }
        }
        

        todo!()
    }
}
