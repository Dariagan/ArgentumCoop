use rand::{rng, seq::SliceRandom};
use rand_pcg::Lcg128Xsl64;


pub fn shuffle_and_into_iter<'a>(used_vec: &'a mut Vec<u32>, arg_rng: Option<&mut Lcg128Xsl64>) -> impl Iterator<Item = u32> + 'a {
  if let Some(rng) = arg_rng {
      used_vec.shuffle(rng);
  } else {
      used_vec.shuffle(&mut rng());
  }
  used_vec.iter_mut().map(|&mut x| x) 
}
