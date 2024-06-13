use std::borrow::{BorrowMut};
use std::thread::{self, JoinHandle};

use fastnoise_lite::{FastNoiseLite, NoiseType};
use godot::{builtin::Dictionary};

use godot::prelude::*;
//use noise::utils::NoiseMap;
use rand::{thread_rng, Rng};
use rand_pcg::Lcg128Xsl64;
use strum::{EnumCount, VariantNames}; use strum_macros::{EnumCount as EnumCountMacro};
use crate::utils::*;
use crate::formation_generator::*;
use crate::tiling::*;
use crate::world_matrix::TileUnidArray;

pub struct FracturedContinentGenerator{}

#[derive(strum_macros::VariantNames, EnumCount)]
#[strum(serialize_all = "snake_case")]
enum Target{ Beach = 0, Lake, Cont, Tree, Bush, Ocean, Cave0, Cave1, Cave2}
impl FracturedContinentGenerator{

    pub fn new() -> Self {
        todo!()
    }
}
//Todo HACERLE UN WRAPPER A FASTNOISELITE PARA Q SE PUEDA SACAR NOISE CON UNSVEC

//TODO LO Q SE TOQUE EN EL GAMEPLAY DEBE SER GUARDADO EN UN DICT(INCLUSO LO DESTRUIDO)
impl IFormationGenerator for FracturedContinentGenerator {
fn generate(mut world: WorldMatrix, origin: UnsVec, size: UnsVec,
    tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary,
) -> WorldMatrix{unsafe{
    
    let world_ptr: SendMutPtr<WorldMatrix> = make_mut_ptr!(world.borrow_mut());
    
    let mut unidordist_mapped2targets: [UnidOrDist; Target::COUNT] = Default::default();
    crate::tiling::fill_targets(&mut unidordist_mapped2targets, Target::VARIANTS, tile_selection);

    let unidordist_mapped2targets: SendPtr<[UnidOrDist; Target::COUNT]> = make_ptr!(&unidordist_mapped2targets);
    //TODO hacer esto un struct ^
    
    //https://github.com/Auburn/FastNoiseLite/tree/master/Rust
    //https://github.com/Razaekel/noise-rs/tree/4fea5f6156bd0b142495a99fe1995502bfe473d6

    let continenter_cutoff: f32 = 0.61*f32::powf((size.length()/1600.0) as f32,0.05);
    let mut continenter=FastNoiseLite::new();continenter.noise_type=NoiseType::OpenSimplex2;
    continenter.set_fractal_lacunarity(Some(2.8));continenter.set_fractal_weighted_strength(Some(0.5));
    continenter.frequency = 0.15/f32::powf(size.length() as f32, 0.995);

    let mut continenter_sampling_offset: UnsVec = UnsVec { lef: 0, right: 0 };
    {
        let center: UnsVec = (origin + size)/2;
        while continenter.get_noise_2d((center+continenter_sampling_offset).lef as f64, (center+continenter_sampling_offset).right as f64) < continenter_cutoff + 0.13{
            continenter_sampling_offset += UnsVec{lef:3, right:3}
        }
    }let continenter_offset: UnsVec = continenter_sampling_offset;

    let continenter = SharedNoise::new(&continenter);

    const PENINSULER_CUTOFF: f32 = -0.1;
    let mut peninsuler=FastNoiseLite::with_seed(seed+1);peninsuler.noise_type=NoiseType::OpenSimplex2;
    peninsuler.set_fractal_gain(Some(0.56));
    peninsuler.frequency = 5.9/f32::powf(size.length() as f32, 0.995);
    let peninsuler = SharedNoise::new(&peninsuler);

    const BIG_LAKER_CUTOFF: f32 = 0.33;
    let mut big_laker=FastNoiseLite::with_seed(seed+2);big_laker.noise_type=NoiseType::ValueCubic;
    big_laker.frequency = 40.0/f32::powf(size.length() as f32, 0.995);
    let big_laker = SharedNoise::new(&big_laker);
    const SMALL_LAKER_CUTOFF: f32 = 0.25; 
    let mut small_laker=FastNoiseLite::with_seed(seed+3);small_laker.noise_type=NoiseType::ValueCubic;
    small_laker.frequency = 80.0/f32::powf(size.length() as f32, 0.995);
    let small_laker = SharedNoise::new(&small_laker);

    const BEACHER_CUTOFF: f32 = 0.8; 
    let mut big_beacher=FastNoiseLite::with_seed(seed+4);big_beacher.noise_type=NoiseType::OpenSimplex2S;
    big_beacher.frequency = 4.3/f32::powf(size.length() as f32, 0.995);
    let big_beacher = SharedNoise::new(&big_beacher);

    let mut small_beacher=FastNoiseLite::with_seed(seed+5);small_beacher.noise_type=NoiseType::OpenSimplex2S;
    small_beacher.set_fractal_octaves(Some(3));
    small_beacher.frequency = 8.0/f32::powf(size.length() as f32, 0.995);
    let small_beacher = SharedNoise::new(&small_beacher);

    const FORESTER_CUTOFF: f32 = 4.3; 
    let mut forester=FastNoiseLite::with_seed(seed+6);forester.noise_type=NoiseType::OpenSimplex2;
    forester.set_fractal_lacunarity(Some(3.0));forester.set_fractal_gain(Some(0.77));
    forester.frequency = 1.6/f32::powf(size.length() as f32, 0.995);
    let forester = SharedNoise::new(&forester);

    const N_THREADS: usize = 4;
    let mut threads: [Option<JoinHandle<()>>; N_THREADS] = Default::default();
    let mut rngs: [Pcg64; N_THREADS] = core::array::from_fn(|i| Seeder::from(seed+i as i32).make_rng());
    let mut rngs: SendMutPtr<[Pcg64; N_THREADS]> = make_mut_ptr!(&mut rngs);

    for thread_i in 0..N_THREADS {threads[thread_i] = Some(thread::spawn(move || {
    let hori_range = (((thread_i*size.lef as usize)/N_THREADS) as u32, (((thread_i+1)*size.lef as usize)/N_THREADS) as u32);
    for rel_coords in (hori_range.0..hori_range.1).flat_map(|i| (0..size.right).map(move |j| UnsVec::from((i,j)))){
    
        let mut tiles_2b_placed: TileUnidArray = Default::default();//hacer TileUnidArray un struct real y agregarle un método fácil para meterle un TileUnid y TileZlevel
        let continenter=continenter;let peninsuler=peninsuler;let big_laker=big_laker;let small_laker=small_laker;let big_beacher=big_beacher;let small_beacher=small_beacher;let forester=forester;
        
        let (continentness, continental): (f32, bool) = val_surpasses_cutoff(get_continentness(continenter, rel_coords, size, None, Some(continenter_offset)), continenter_cutoff);

        let (peninsulerness, peninsuler_caved) : (f32, bool) = noise_surpasses_cutoff(peninsuler, rel_coords, PENINSULER_CUTOFF);

        let rng: &mut Lcg128Xsl64 = rngs.drf().get_unchecked_mut(thread_i);
        if continental && peninsuler_caved{
            tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Cont as usize).get_a_nid(rng));

            let (beachness, beach) = val_surpasses_cutoff(get_beachness(rel_coords, big_beacher, small_beacher, (continentness, continenter_cutoff), (peninsulerness, PENINSULER_CUTOFF)), BEACHER_CUTOFF);
            if beach {
                tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Beach as usize).get_a_nid(rng))
            }
            else {//TODO HACER UN MATCH STATEMENT
                let away_from_coast = continentness > continenter_cutoff+0.01 && peninsulerness > PENINSULER_CUTOFF+0.27;

                let lake = is_lake(rel_coords, (big_laker, BIG_LAKER_CUTOFF), (small_laker, SMALL_LAKER_CUTOFF), beachness);

                if lake && away_from_coast{
                    tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Lake as usize).get_a_nid(rng))
                }
                else {

                }
            }
        }
        else {
            tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Ocean as usize).get_a_nid(rng));

        }

        overwrite_all_tiles_at_coord(world_ptr, (origin, rel_coords), &tiles_2b_placed, None);
    }
    }))}  
    
    for thread in threads {if let Some(thread) = thread{let _ = thread.join();}}
    world
    
}}
}
#[inline]
pub fn get_beachness(rel_coords: UnsVec, big_beacher: SharedNoise, small_beacher: SharedNoise, 
    (continentness, continental_cutoff): (f32, f32), (peninsulerness, peninsuler_cutoff): (f32, f32)) -> f32 {unsafe{
    (0.72 + big_beacher.get_noise_2d(rel_coords)/2.3 - f32::powf(continentness-continental_cutoff,0.6))
    .max(0.8 + small_beacher.get_noise_2d(rel_coords)/2.3 - f32::powf(peninsulerness-peninsuler_cutoff,0.45))
}}

#[inline]
pub fn is_lake(rel_coords: UnsVec, (big_laker, big_laker_cutoff): (SharedNoise, f32), (small_laker, small_laker_cutoff): (SharedNoise, f32), 
    beachness: f32) -> bool {unsafe{
    ((small_laker.get_noise_2d(rel_coords)+1.0)*0.65 - beachness > small_laker_cutoff) || (((big_laker.get_noise_2d(rel_coords) + 1.0)*0.65) - beachness > big_laker_cutoff)
}}