use std::thread::{self, JoinHandle};

use fastnoise_lite::{FastNoiseLite, NoiseType};
use godot::{builtin::Dictionary};

use godot::prelude::*;
use ::noise::MultiFractal;
use strum_macros::{EnumString}; use strum::{EnumCount, VariantNames}; use strum_macros::{EnumCount as EnumCountMacro};
use crate::formation_generator::{make_ptr, SendPtr};
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
        unsafe{
        let mut nids_mapped_to_targets: [NidOrDist; Target::COUNT] = Default::default();
        crate::tiling::fill_targets(&mut nids_mapped_to_targets, Target::VARIANTS, tile_selection);
        
        //https://github.com/Auburn/FastNoiseLite/tree/master/Rust
        //
        extern crate noise;

        let continenter_cutoff: f32 = 0.61*f32::powf((size.length()/1600.0) as f32,0.05);
        let mut continenter=FastNoiseLite::new();continenter.noise_type=NoiseType::OpenSimplex2;continenter.seed=seed;
        continenter.set_fractal_lacunarity(Some(2.8));continenter.set_fractal_weighted_strength(Some(0.5));
        let continenter_ptr: SendPtr<FastNoiseLite> = make_ptr!(&continenter);

        const PENINSULER_CUTOFF:f32 = -0.1;
        let mut peninsuler=FastNoiseLite::new();peninsuler.noise_type=NoiseType::OpenSimplex2;peninsuler.seed=seed+1;
        peninsuler.set_fractal_gain(Some(0.56));
        let continenter_ptr: SendPtr<FastNoiseLite> = make_ptr!(&continenter);

        const BIG_LAKER_CUTOFF:f32 = 0.33;
        let mut big_laker=FastNoiseLite::new();big_laker.noise_type=NoiseType::ValueCubic;big_laker.seed=seed+2;
        let continenter_ptr: SendPtr<FastNoiseLite> = make_ptr!(&continenter);
        const SMALL_LAKER_CUTOFF: f32 = 0.25; 
        let mut small_laker=FastNoiseLite::new();small_laker.noise_type=NoiseType::ValueCubic;small_laker.seed=seed+3;
        let continenter_ptr: SendPtr<FastNoiseLite> = make_ptr!(&continenter);

        const BEACHER_CUTOFF: f32 = 0.8; 
        let mut big_beacher=FastNoiseLite::new();big_beacher.noise_type=NoiseType::OpenSimplex2S;big_beacher.seed=seed+4;
        let continenter_ptr: SendPtr<FastNoiseLite> = make_ptr!(&continenter);
        let mut small_beacher=FastNoiseLite::new();small_beacher.noise_type=NoiseType::OpenSimplex2S;small_beacher.seed=seed+5;
        small_beacher.set_fractal_octaves(Some(3));
        let continenter_ptr: SendPtr<FastNoiseLite> = make_ptr!(&continenter);

        const FORESTER_CUTOFF: f32 = 4.3; 
        let mut forester=FastNoiseLite::new();forester.noise_type=NoiseType::OpenSimplex2;forester.seed=seed+6;
        forester.set_fractal_lacunarity(Some(3.0));forester.set_fractal_gain(Some(0.77));


        const N_THREADS: usize = 16;
        let mut threads: [Option<JoinHandle<()>>; N_THREADS] = Default::default();

        for thread_i in 0..N_THREADS {threads[thread_i] = Some(thread::spawn(move || {
            
            let hori_range = ((thread_i*size.lef as usize/N_THREADS) as u32, (thread_i*size.lef as usize/N_THREADS) as u32);
            for rel_coords in (hori_range.0..hori_range.1).zip(0..size.right).map(UnsVec::from) {
                
                let continenter = continenter_ptr;
                if (&*continenter.0).get_noise_2d(rel_coords.lef as f64, rel_coords.right as f64) > continenter_cutoff {
                    todo!()
                }
                todo!()
            }
        }))}  
        
        for thread in threads {
            if let Some(thread) = thread{
                let _ = thread.join();
            }
        }
        }      

        todo!()
    }
}

