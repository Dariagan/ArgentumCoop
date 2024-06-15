use std::borrow::{BorrowMut};
use std::thread::{self, JoinHandle};

use fastnoise_lite::{FastNoiseLite, FractalType, NoiseType};
use godot::{builtin::Dictionary};

use godot::prelude::*;
use gxhash::HashSet;
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

  //https://auburn.github.io/FastNoiseLite/ para mirar noise maps con slider

  let continenter_cutoff: f32 = 0.61*f32::powf((size.length()/1600.0) as f32,0.05);
  const PENINSULER_CUTOFF: f32 = -0.1;
  const BIG_LAKER_CUTOFF: f32 = 0.33;
  const SMALL_LAKER_CUTOFF: f32 = 0.25; 
  const BEACHER_CUTOFF: f32 = 0.8; 
  const FORESTER_CUTOFF: f32 = 4.3; 
  
  let mut continenter=FastNoiseLite::with_seed(seed);continenter.noise_type=NoiseType::OpenSimplex2;
  let mut peninsuler=FastNoiseLite::with_seed(seed+1);peninsuler.noise_type=NoiseType::OpenSimplex2;
  let mut big_beacher=FastNoiseLite::with_seed(seed+4);big_beacher.noise_type=NoiseType::OpenSimplex2S;
  let mut small_beacher=FastNoiseLite::with_seed(seed+5);small_beacher.noise_type=NoiseType::OpenSimplex2S;
  let mut big_laker=FastNoiseLite::with_seed(seed+2);big_laker.noise_type=NoiseType::ValueCubic;
  let mut small_laker=FastNoiseLite::with_seed(seed+3);small_laker.noise_type=NoiseType::ValueCubic;
  let mut forester=FastNoiseLite::with_seed(seed+6);forester.noise_type=NoiseType::OpenSimplex2;
  
  continenter.frequency = 0.15/size.length_f32().powf(0.995);
  peninsuler.frequency = 5.0/size.length_f32().powf(0.995);
  big_beacher.frequency = 4.3/size.length_f32().powf(0.995);
  small_beacher.frequency = 8.0/size.length_f32().powf(0.995);
  big_laker.frequency = 40.0/size.length_f32().powf(0.995);
  small_laker.frequency = 80.0/size.length_f32().powf(0.995);
  forester.frequency = 1.6/size.length_f32().powf(0.995);
  
  continenter.set_fractal_type(Some(FractalType::FBm)); continenter.set_fractal_octaves(Some(5));
  peninsuler.set_fractal_type(Some(FractalType::FBm)); peninsuler.set_fractal_octaves(Some(5));
  big_beacher.set_fractal_type(Some(FractalType::FBm)); big_beacher.set_fractal_octaves(Some(5));
  small_beacher.set_fractal_type(Some(FractalType::FBm)); small_beacher.set_fractal_octaves(Some(3));
  big_laker.set_fractal_type(Some(FractalType::FBm)); big_laker.set_fractal_octaves(Some(5));
  small_laker.set_fractal_type(Some(FractalType::FBm)); small_laker.set_fractal_octaves(Some(5));
  forester.set_fractal_type(Some(FractalType::FBm)); forester.set_fractal_octaves(Some(5));
  
  continenter.set_fractal_lacunarity(Some(2.8));
  forester.set_fractal_lacunarity(Some(3.0));

  continenter.set_fractal_weighted_strength(Some(0.5));
  
  peninsuler.set_fractal_gain(Some(0.56));    
  forester.set_fractal_gain(Some(0.77));
  
  godot_print!("con_co:{continenter_cutoff} con_freq:{} pen_freq:{} blakf:{} slakf:{} bbfreq:{} sbfreq:{}", 
    continenter.frequency, peninsuler.frequency, big_laker.frequency, small_laker.frequency, big_beacher.frequency, small_beacher.frequency);

  let mut continenter_offset: UnsVec = UnsVec { lef: 0, right: 0 };
  {
    let center: UnsVec = origin + size/2;
    while continenter.get_noise_2d((center+continenter_offset).lef as f64, (center+continenter_offset).right as f64) < continenter_cutoff + 0.13{
      continenter_offset += UnsVec{lef:3, right:3}
    }
  }let continenter_offset: UnsVec = continenter_offset;

  let continenter = SharedNoise::new(&continenter);
  let peninsuler = SharedNoise::new(&peninsuler);
  let big_laker = SharedNoise::new(&big_laker);
  let small_laker = SharedNoise::new(&small_laker);
  let big_beacher = SharedNoise::new(&big_beacher);
  let small_beacher = SharedNoise::new(&small_beacher);
  let forester = SharedNoise::new(&forester);

  const N_THREADS: usize = 16;
  let mut threads: [Option<JoinHandle<()>>; N_THREADS] = Default::default();
  let mut rngs: [Pcg64; N_THREADS] = core::array::from_fn(|i| Seeder::from(seed+i as i32).make_rng());
  let mut rngs: SendMutPtr<[Pcg64; N_THREADS]> = make_mut_ptr!(&mut rngs);

  let mut bushes: [HashSet<UnsVec>; N_THREADS] = Default::default();
  let mut bushes: SendMutPtr<[HashSet<UnsVec>; N_THREADS]> = make_mut_ptr!(&mut bushes);
  let mut all_bushes: HashSet<UnsVec> = Default::default();

  let mut trees: [HashSet<UnsVec>; N_THREADS] = Default::default();
  let mut trees: SendMutPtr<[HashSet<UnsVec>; N_THREADS]> = make_mut_ptr!(&mut trees);
  let mut all_trees: HashSet<UnsVec> = Default::default();

  for thread_i in 0..N_THREADS {threads[thread_i] = Some(thread::spawn(move || {
  let hori_range = (((thread_i*size.lef as usize)/N_THREADS) as u32, (((thread_i+1)*size.lef as usize)/N_THREADS) as u32);
  for rel_coords in (hori_range.0..hori_range.1).flat_map(|i| (0..size.right).map(move |j| UnsVec::from((i,j)))){
  
    let mut tiles_2b_placed: TileUnidArray = Default::default();
    let continenter=continenter;let peninsuler=peninsuler;let big_laker=big_laker;let small_laker=small_laker;let big_beacher=big_beacher;let small_beacher=small_beacher;let forester=forester;
    let trees = trees.drf().get_unchecked_mut(thread_i); let bushes = bushes.drf().get_unchecked_mut(thread_i);

    let (continentness, continental): (f32, bool) = val_surpasses_cutoff(get_continentness(continenter, rel_coords, size, None, Some(continenter_offset)), continenter_cutoff);

    let (peninsulerness, peninsuler_caved) : (f32, bool) = noise_surpasses_cutoff(peninsuler, rel_coords, PENINSULER_CUTOFF);

    let rng: &mut Lcg128Xsl64 = rngs.drf().get_unchecked_mut(thread_i);
    if continental && peninsuler_caved{

      let (beachness, beach) = val_surpasses_cutoff(get_beachness(rel_coords, big_beacher, small_beacher, (continentness, continenter_cutoff), (peninsulerness, PENINSULER_CUTOFF)), BEACHER_CUTOFF);
      if beach {
        tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Beach as usize).get_unid(rng))
      }
      else {//TODO HACER UN MATCH STATEMENT
        let away_from_coast = continentness > continenter_cutoff+0.01 && peninsulerness > PENINSULER_CUTOFF+0.27;

        let lake = is_lake(rel_coords, (big_laker, BIG_LAKER_CUTOFF), (small_laker, SMALL_LAKER_CUTOFF), beachness);

        if lake && away_from_coast{
          tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Lake as usize).get_unid(rng))
        }
        else {
          tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Cont as usize).get_unid(rng));

          if beachness < BEACHER_CUTOFF - 0.05 {
            let good_roll = rng.gen_range(0.0..=4.0) + forester.get_noise_2d(rel_coords)*1.4 > FORESTER_CUTOFF;
            let rogue_tree = rng.gen_bool(1.0/1000.0);

            let place_tree = (good_roll || rogue_tree) && clear_of(trees, rel_coords, 3, false);

            if place_tree{
              trees.insert(rel_coords);
              tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Tree as usize).get_unid(rng));
            }
            
            else if rng.gen_bool(1.0/400.0) && clear_of(&bushes, rel_coords, 1, false){
              bushes.insert(rel_coords);
              tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Bush as usize).get_unid(rng));
            }
          }
        }
      }
    }
    else {
      tiles_2b_placed.assign_unid_unchecked(unidordist_mapped2targets.drf().get_unchecked(Target::Ocean as usize).get_unid(rng));
    }
    overwrite_all_tiles_at_coord(world_ptr, (origin, rel_coords), &tiles_2b_placed, None);
  }
  }))}  
  for (thread_i, thread) in threads.into_iter().enumerate() {
    let _ = thread.unwrap_unchecked().join();
    all_trees.extend(trees.drf().get_unchecked_mut(thread_i).iter());
    all_bushes.extend(bushes.drf().get_unchecked_mut(thread_i).iter());
  }



  world
  
}}
}
#[inline]
pub fn get_beachness(rel_coords: UnsVec, big_beacher: SharedNoise, small_beacher: SharedNoise, 
  (continentness, continental_cutoff): (f32, f32), (peninsulerness, peninsuler_cutoff): (f32, f32)) -> f32 {unsafe{
  (0.72 + big_beacher.get_noise_2d(rel_coords)/2.3 - (continentness-continental_cutoff).powf(0.6))
  .max(0.8 + small_beacher.get_noise_2d(rel_coords)/2.3 - (peninsulerness-peninsuler_cutoff).powf(0.45))
}}
#[inline]
pub fn is_lake(rel_coords: UnsVec, (big_laker, big_laker_cutoff): (SharedNoise, f32), (small_laker, small_laker_cutoff): (SharedNoise, f32), 
  beachness: f32) -> bool {unsafe{
  ((small_laker.get_noise_2d(rel_coords)+1.0) * 0.65 - beachness > small_laker_cutoff) 
  || (((big_laker.get_noise_2d(rel_coords)+1.0) * 0.65) - beachness > big_laker_cutoff)
}}


// use std::cmp::min;

// fn place_dungeon_entrances(
//     dungeons_to_place: u8,
//     size: UnsVec,
//     rng: &mut Pcg64,
//     continental_cutoff: f64,
//     peninsuler: &SharedNoise,
//     peninsuler_cutoff: f32,
//     trees: &HashSet<UnsVec>,
//     m_origin: UnsVec,
// ) {
//     const MAX_TRIES: u64 = 1_000_000;
  
//     let dungeons_to_place = min(dungeons_to_place, N_CAVES as u8);
//     let mut placed_dungeons_coords = Vec::with_capacity(dungeons_to_place as usize);
  
//     let mut min_distance_multiplier = 1.0;
//     let mut tries_count = 1;
  
//     while placed_dungeons_coords.len() < dungeons_to_place as usize {
//         let r_coords = UnsVec {
//             lef: rng.randi_range(0, size.lef),
//             right: rng.randi_range(0, size.right),
//         };
    
//         if get_continentness(r_coords) > continental_cutoff + 0.005
//             && peninsuler.get_noise_2d(r_coords) > peninsuler_cutoff + 0.1
//             && !is_lake(r_coords)
//             && clear_of(trees, r_coords, 3, true)
//         {
//             let min_distance_between_dungeons = size.length() as f64 * 0.25 * min_distance_multiplier;
      
//             let is_too_close = |coord: &UnsVec| r_coords.distance_to(coord) < min_distance_between_dungeons;
      
//             if placed_dungeons_coords.iter().all(|coord| !is_too_close(coord)) {
//                 placed_dungeons_coords.push(r_coords);
        
        
        
//                 argentum_tile_map.place_formation_tile(m_origin, r_coords, tile_uid);
//                 UtilityFunctions::print(r_coords);
//             } else {
//                 min_distance_multiplier = (1500.0 / tries_count as f64).clamp(0.0, 1.0);
//             }
//         }
    
//         if tries_count > MAX_TRIES {
//             panic!("Dungeon placement condition unmeetable! (FracturedContinentGenerator::placeDungeonEntrances())");
//             break;
//         }
    
//         tries_count += 1;
//     }
// }
