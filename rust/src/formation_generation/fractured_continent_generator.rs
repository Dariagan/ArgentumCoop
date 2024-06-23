use std::borrow::BorrowMut;
use gxhash::HashSet;
use super::*;

//RECOMMENDED SIZE = 2500X2500
pub struct FracturedContinentGenerator{}

#[derive(strum_macros::VariantNames, EnumCount)]
#[strum(serialize_all = "snake_case")]//no poner targets nuevos al final (ahí van las caves)
enum Target{ Beach = 0, Lake, Cont, Tree, Bush, Ocean, Cave0, Cave1, Cave2}


const BIG_LAKER_CUTOFF: f32 = 1.23;
const SMALL_LAKER_CUTOFF: f32 = 1.15; 
const BEACHER_CUTOFF: f32 = 1.; 
const FORESTER_CUTOFF: f32 = 5.4; 

//TODO LO Q SE TOQUE EN EL GAMEPLAY DEBE SER GUARDADO EN UN DICT(INCLUSO LO DESTRUIDO)
impl IFormationGenerator for FracturedContinentGenerator {
fn generate(world: &mut WorldMatrix, origin: UnsVec, size: UnsVec,
tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary,
) {unsafe{
//NOTA: SI ES MUY ALTO SE QUEDA INFINITAMENTE EN EL WHILE DE ABAJO
CONTINENTER_CUTOFF = 1.77;

let world_ptr: SendMutPtr<WorldMatrix> = make_mut_ptr!(world.borrow_mut());

let mut unidordist_mapped2targets: [UnidOrDist; Target::COUNT] = Default::default();
crate::tiling::fill_targets(&mut unidordist_mapped2targets, Target::VARIANTS, tile_selection);

let unidordist_mped2targets: SendPtr<[UnidOrDist; Target::COUNT]> = make_ptr!(&unidordist_mapped2targets);
//TODO hacer esto un struct ^

//https://github.com/Auburn/FastNoiseLite/tree/master/Rust
//https://github.com/Razaekel/noise-rs/tree/4fea5f6156bd0b142495a99fe1995502bfe473d6

//https://auburn.github.io/FastNoiseLite/ para mirar noise maps con slider

let mut continenter: FastNoiseLite=FastNoiseLite::with_seed(seed);continenter.noise_type=NoiseType::OpenSimplex2;
let mut peninsuler: FastNoiseLite=FastNoiseLite::with_seed(seed+1);peninsuler.noise_type=NoiseType::OpenSimplex2;
let mut big_beacher: FastNoiseLite=FastNoiseLite::with_seed(seed+4);big_beacher.noise_type=NoiseType::OpenSimplex2S;
let mut small_beacher: FastNoiseLite=FastNoiseLite::with_seed(seed+5);small_beacher.noise_type=NoiseType::OpenSimplex2S;
let mut big_laker: FastNoiseLite=FastNoiseLite::with_seed(seed+2);big_laker.noise_type=NoiseType::ValueCubic;
let mut small_laker: FastNoiseLite=FastNoiseLite::with_seed(seed+3);small_laker.noise_type=NoiseType::ValueCubic;
let mut forester: FastNoiseLite=FastNoiseLite::with_seed(seed+6);forester.noise_type=NoiseType::OpenSimplex2;

continenter.frequency = 0.15/size.length_f32().powf(0.995);
peninsuler.frequency = 3.4/size.length_f32().powf(0.995);
big_beacher.frequency = 3.44/size.length_f32().powf(0.995);
small_beacher.frequency = 6.4/size.length_f32().powf(0.995);
big_laker.frequency = 32.0/size.length_f32().powf(0.995);
small_laker.frequency = 64.0/size.length_f32().powf(0.995);
forester.frequency = 0.6/size.length_f32().powf(0.995);

continenter.set_fractal_type(Some(FractalType::FBm)); continenter.set_fractal_octaves(Some(5));
peninsuler.set_fractal_type(Some(FractalType::FBm)); peninsuler.set_fractal_octaves(Some(5));
big_beacher.set_fractal_type(Some(FractalType::FBm)); big_beacher.set_fractal_octaves(Some(5));
small_beacher.set_fractal_type(Some(FractalType::FBm)); small_beacher.set_fractal_octaves(Some(3));
big_laker.set_fractal_type(Some(FractalType::FBm)); big_laker.set_fractal_octaves(Some(5));
small_laker.set_fractal_type(Some(FractalType::FBm)); small_laker.set_fractal_octaves(Some(5));
forester.set_fractal_type(Some(FractalType::FBm)); forester.set_fractal_octaves(Some(5));

continenter.set_fractal_lacunarity(Some(2.8));
continenter.set_fractal_weighted_strength(Some(0.5));

peninsuler.set_fractal_gain(Some(0.56));    

forester.set_fractal_lacunarity(Some(3.0));
forester.set_fractal_gain(Some(0.77));


let continenter = SharedNoise::new(&continenter);
let peninsuler = SharedNoise::new(&peninsuler);
let big_laker = SharedNoise::new(&big_laker);
let small_laker = SharedNoise::new(&small_laker);
let big_beacher = SharedNoise::new(&big_beacher);
let small_beacher = SharedNoise::new(&small_beacher);
let forester = SharedNoise::new(&forester);

let mut continenter_offset: UnsVec = UnsVec { lef: 0, right: 0 };
{
  let center: UnsVec = origin + size/2;//se traba acá
  while continenter.get_noise_2d(center + continenter_offset) < CONTINENTER_CUTOFF +0.01 {
    continenter_offset += UnsVec{lef:3, right:3}
  }
}let continenter_offset: UnsVec = continenter_offset;

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
  //-------------THREAD-LOCAL-CODE-STARTS HERE------------------
  let hori_range = (((thread_i*size.lef as usize)/N_THREADS) as u32, (((thread_i+1)*size.lef as usize)/N_THREADS) as u32);
  for rel_coords in (hori_range.0..hori_range.1).flat_map(|i| (0..size.right).map(move |j| UnsVec::from((i,j)))){
    
  let mut tiles_2b_placed: TileUnidArray = Default::default();
  let continenter=continenter;let peninsuler=peninsuler;let big_laker=big_laker;let small_laker=small_laker;let big_beacher=big_beacher;let small_beacher=small_beacher;let forester=forester;
  let trees = trees.drf().get_unchecked_mut(thread_i); let bushes = bushes.drf().get_unchecked_mut(thread_i);
  
  
  let (continentness, continental): (f32, bool) = 
  val_surpasses_cutoff(peninsuler.get_noise_2d(rel_coords)*calc_continentness(continenter, rel_coords, size, None, Some(continenter_offset)), CONTINENTER_CUTOFF);
  
        
  let rng: &mut Lcg128Xsl64 = rngs.drf().get_unchecked_mut(thread_i);
  if continental{
    tiles_2b_placed.assign_unid(unidordist_mped2targets.drf().get_unchecked(Target::Cont as usize).get_unid(rng));
    
    let (beachness, beach) = val_surpasses_cutoff(calc_beachness(rel_coords, big_beacher, small_beacher, continentness), BEACHER_CUTOFF);
    if beach {
      tiles_2b_placed.assign_unid(unidordist_mped2targets.drf().get_unchecked(Target::Beach as usize).get_unid(rng))
    }//else rocky
    else {
      
      let lake =  calc_lakeness(rel_coords, big_laker, small_laker, beachness, continentness);
      
      if lake {
        tiles_2b_placed.assign_unid(unidordist_mped2targets.drf().get_unchecked(Target::Lake as usize).get_unid(rng))
      }
      else if beachness < BEACHER_CUTOFF - 0.05{
        let good_roll = rng.gen_range(0.0..=4.0) + forester.get_noise_2d(rel_coords)*1.4 > FORESTER_CUTOFF;
        let rogue_tree = rng.gen_bool(1.0/1000.0);
        
        let tree = (good_roll || rogue_tree) && clear_of(trees, rel_coords, 3, false);
        
        if tree{
          trees.insert(rel_coords);
          tiles_2b_placed.assign_unid(unidordist_mped2targets.drf().get_unchecked(Target::Tree as usize).get_unid(rng));
        }
        else if rng.gen_bool(1.0/400.0) && clear_of(&bushes, rel_coords, 1, false){
          bushes.insert(rel_coords);
          tiles_2b_placed.assign_unid(unidordist_mped2targets.drf().get_unchecked(Target::Bush as usize).get_unid(rng));
        }
      }
    }
  } else {
    tiles_2b_placed.assign_unid(unidordist_mped2targets.drf().get_unchecked(Target::Ocean as usize).get_unid(rng));
  }
  overwrite_all_tiles_at_coord(world_ptr, (origin, rel_coords), &tiles_2b_placed, None);
}
}))}//END OF THREAD CODE  
for (thread_i, thread) in threads.into_iter().enumerate() {
  let _ = thread.unwrap_unchecked().join();
  all_trees.extend(trees.drf().get_unchecked_mut(thread_i).iter());
  all_bushes.extend(bushes.drf().get_unchecked_mut(thread_i).iter());
}
{// place_dungeon_entrances
  const MAX_TRIES: u64 = 1_000_000;

  const DUNGEONS_TO_PLACE: usize = Target::COUNT - Target::Cave0 as usize;
  let mut placed_dungeons_coords: [UnsVec; DUNGEONS_TO_PLACE] = Default::default();

  let mut min_distance_multiplier = 1.0;
  let mut tries_count = 1;
  let rng = rngs.drf().get_unchecked_mut(0);

  let mut placed_count: usize = 0;
  while placed_count < placed_dungeons_coords.len() {
    let r_coords = UnsVec {
        lef: rng.gen_range(size.lef/10..size.lef*9/10),
        right: rng.gen_range(size.right/10..size.right*9/10),
      };
  
    let continentness = calc_continentness(continenter, r_coords, size, None, Some(continenter_offset));
    if continentness > CONTINENTER_CUTOFF + 0.005 
      && !calc_lakeness(r_coords, big_laker, small_laker, calc_beachness(r_coords, big_beacher, small_beacher, continentness), continentness)
      && clear_of(&all_trees, r_coords, 3, true)
    {
      let min_distance_between_dungeons = size.length() as f64 * 0.25 * min_distance_multiplier;

      let is_separated_enough = |coord: &UnsVec| r_coords.distance_to(coord) > min_distance_between_dungeons;

      if placed_dungeons_coords.iter().all(|coord| is_separated_enough(coord)) {
        *placed_dungeons_coords.get_unchecked_mut(placed_count) = r_coords;
        overwrite_formation_tile(world_ptr, (origin, r_coords), unidordist_mped2targets.drf().get_unchecked(Target::Cave0 as usize + placed_count).get_unid(rng), None);
        placed_count += 1;
        godot_print!("{}", r_coords);
      } else {
        min_distance_multiplier = (1500.0 / tries_count as f64).clamp(0.0, 1.0);
      }
    }
    if tries_count > MAX_TRIES {panic!("Dungeon placement condition unmeetable!");}

    tries_count += 1;
  }
}// end of place_dungeon_entrances
}}
}
#[inline] //se puede mejorar viendo el peninsulerness value
pub fn calc_beachness(rel_coords: UnsVec, big_beacher: SharedNoise, small_beacher: SharedNoise, 
  continentness: f32) -> f32 {unsafe {
    big_beacher.get_noise_2d(rel_coords).max(small_beacher.get_noise_2d(rel_coords)) - (continentness-CONTINENTER_CUTOFF).powf(0.6)
          
  }
}
#[inline]
pub fn calc_lakeness(rel_coords: UnsVec, big_laker: SharedNoise, small_laker: SharedNoise, beachness: f32, continentness: f32) -> bool {
  (small_laker.get_noise_2d(rel_coords) * 0.65 - beachness + continentness/7.0> SMALL_LAKER_CUTOFF) 
  || ((big_laker.get_noise_2d(rel_coords) * 0.65) - beachness + continentness/7.0> BIG_LAKER_CUTOFF) 
}
  
  
  static mut CONTINENTER_CUTOFF: f32 = 0.0;