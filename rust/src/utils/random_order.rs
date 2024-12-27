use rand::{seq::SliceRandom, thread_rng};
use rand_pcg::Lcg128Xsl64;

use rand::{Rng, rngs::ThreadRng};

pub fn shuffle_and_into_iter<'a>(used_vec: &'a mut Vec<u32>, rng: Option<&mut Lcg128Xsl64>) -> impl Iterator<Item = u32> + 'a {
  if let Some(rng) = rng {
      used_vec.shuffle(rng);
  } else {
      used_vec.shuffle(&mut thread_rng());
  }
  used_vec.iter_mut().map(|&mut x| x) 
}
