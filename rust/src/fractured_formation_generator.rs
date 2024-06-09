use std::borrow::{Borrow, BorrowMut};
use std::mem::{self, MaybeUninit};
use std::thread::{self, JoinHandle};

use fastnoise_lite::{FastNoiseLite, NoiseType};
use godot::{builtin::Dictionary};

use godot::prelude::*;
use strum_macros::{EnumString}; use strum::{EnumCount, VariantNames}; use strum_macros::{EnumCount as EnumCountMacro};
use crate::{formation_generator::*, world_matrix};
use crate::uns_vec;
use crate::world_matrix::*;
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
        
        let world: SendMutPtr<WorldMatrix> = make_mut_ptr!(world.borrow_mut());
        
        let mut nids_mapped_to_targets: [NidOrDist; Target::COUNT] = Default::default();
        crate::tiling::fill_targets(&mut nids_mapped_to_targets, Target::VARIANTS, tile_selection);
        let nids_mapped_to_targets: SendPtr<[NidOrDist; Target::COUNT]> = make_ptr!(&nids_mapped_to_targets);
        
        //https://github.com/Auburn/FastNoiseLite/tree/master/Rust
        //

        let continenter_cutoff: f32 = 0.61*f32::powf((size.length()/1600.0) as f32,0.05);
        let mut continenter=FastNoiseLite::new();continenter.noise_type=NoiseType::OpenSimplex2;continenter.seed=seed;
        continenter.set_fractal_lacunarity(Some(2.8));continenter.set_fractal_weighted_strength(Some(0.5));
        let continenter: SendPtr<FastNoiseLite> = make_ptr!(&continenter);

        const PENINSULER_CUTOFF:f32 = -0.1;
        let mut peninsuler=FastNoiseLite::new();peninsuler.noise_type=NoiseType::OpenSimplex2;peninsuler.seed=seed+1;
        peninsuler.set_fractal_gain(Some(0.56));
        let peninsuler: SendPtr<FastNoiseLite> = make_ptr!(&peninsuler);

        const BIG_LAKER_CUTOFF:f32 = 0.33;
        let mut big_laker=FastNoiseLite::new();big_laker.noise_type=NoiseType::ValueCubic;big_laker.seed=seed+2;
        let big_laker: SendPtr<FastNoiseLite> = make_ptr!(&big_laker);
        const SMALL_LAKER_CUTOFF: f32 = 0.25; 
        let mut small_laker=FastNoiseLite::new();small_laker.noise_type=NoiseType::ValueCubic;small_laker.seed=seed+3;
        let small_laker: SendPtr<FastNoiseLite> = make_ptr!(&small_laker);

        const BEACHER_CUTOFF: f32 = 0.8; 
        let mut big_beacher=FastNoiseLite::new();big_beacher.noise_type=NoiseType::OpenSimplex2S;big_beacher.seed=seed+4;
        let big_beacher: SendPtr<FastNoiseLite> = make_ptr!(&big_beacher);
        let mut small_beacher=FastNoiseLite::new();small_beacher.noise_type=NoiseType::OpenSimplex2S;small_beacher.seed=seed+5;
        small_beacher.set_fractal_octaves(Some(3));
        let small_beacher: SendPtr<FastNoiseLite> = make_ptr!(&small_beacher);

        const FORESTER_CUTOFF: f32 = 4.3; 
        let mut forester=FastNoiseLite::new();forester.noise_type=NoiseType::OpenSimplex2;forester.seed=seed+6;
        forester.set_fractal_lacunarity(Some(3.0));forester.set_fractal_gain(Some(0.77));
        let forester: SendPtr<FastNoiseLite> = make_ptr!(&forester);

        const N_THREADS: usize = 16;
        let mut threads: [Option<JoinHandle<()>>; N_THREADS] = Default::default();

        for thread_i in 0..N_THREADS {threads[thread_i] = Some(thread::spawn(move || {
            
            let hori_range = ((thread_i*size.lef as usize/N_THREADS) as u32, (thread_i*size.lef as usize/N_THREADS) as u32);
            for rel_coords in (hori_range.0..hori_range.1).zip(0..size.right).map(UnsVec::from) {

                let tiles_2b_placed: MaybeUninit::<[(TileTypeNid, TileZLevel); TileZLevel::COUNT]> = MaybeUninit::<[(TileTypeNid, TileZLevel); TileZLevel::COUNT]>::uninit();
                
                let continenter=continenter;let peninsuler=peninsuler;let big_laker=big_laker;let small_laker=small_laker;let big_beacher=big_beacher;let small_beacher=small_beacher;let forester=forester;
                let continental = is_continental(continenter, rel_coords, size, continenter_cutoff, None);

                let peninsuler_caved = nv_surpasses_cutoff(peninsuler, rel_coords, PENINSULER_CUTOFF);

                if continental && peninsuler_caved{
                    *tiles_2b_placed.assume_init().get_unchecked_mut(TileZLevel::Bottom as usize) = nids_mapped_to_targets.drf().get_unchecked(Target::Cont as usize).get_a_nid();
                }
                else {
                    *tiles_2b_placed.assume_init().get_unchecked_mut(TileZLevel::Bottom as usize) = nids_mapped_to_targets.drf().get_unchecked(Target::Ocean as usize).get_a_nid();
                }

                overwrite_formation_tile(world, (origin, rel_coords), *tiles_2b_placed.assume_init().get_unchecked(0), None)
                
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



