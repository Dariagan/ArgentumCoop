use std::{collections::HashMap, ops::{Add, Index, IndexMut}};

use crate::utils::{matrix::DownScalingMatrix, uns_vec::UnsVec};

use super::BeingGenTemplIdAndFac;

#[allow(dead_code)]
#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct SpawnWeight(pub u16);
impl Add for SpawnWeight {
  type Output = SpawnWeight;
  fn add(self, other: Self) -> Self::Output {SpawnWeight(self.0 + other.0)}
}

pub struct SpawnWeightsMatrix{ds_matrix: DownScalingMatrix<HashMap<BeingGenTemplIdAndFac, SpawnWeight>>}

#[allow(dead_code)]
impl SpawnWeightsMatrix{
  
  pub fn new(size_to_downscale_from: UnsVec, downscale_factor: u32) -> Self {
    Self { ds_matrix: DownScalingMatrix::new(size_to_downscale_from, downscale_factor) }
  }
  pub fn overwrite_at(&mut self, coords: UnsVec, being_gen_templ_fac: BeingGenTemplIdAndFac, new_weight: SpawnWeight) {
    self.ds_matrix.get_unchecked_mut(coords).insert(being_gen_templ_fac, new_weight);
  }
  pub fn increase_at(&mut self, coords: UnsVec, bein_gen_templ_fac: BeingGenTemplIdAndFac, added_weight: SpawnWeight) {
    let hash_map = self.ds_matrix.get_unchecked_mut(coords);
    hash_map.entry(bein_gen_templ_fac)
      .and_modify(|existing_weight| *existing_weight = *existing_weight + added_weight)
      .or_insert(added_weight);
  }

  pub fn get_unchk_no_downscale(&self, coords: UnsVec) -> &HashMap<BeingGenTemplIdAndFac, SpawnWeight>{
    self.ds_matrix.index(coords)
  }
  pub fn get_unchk_mut_no_downscale(&mut self, coords: UnsVec) -> &mut HashMap<BeingGenTemplIdAndFac, SpawnWeight>{
    self.ds_matrix.index_mut(coords)
  }
  pub fn get_unchk_mut(&mut self, coords: UnsVec) -> &mut HashMap<BeingGenTemplIdAndFac, SpawnWeight>{
    self.ds_matrix.get_unchecked_mut(coords)
  }
  pub fn get(&self, coords: UnsVec) -> Option<&HashMap<BeingGenTemplIdAndFac, SpawnWeight>>{
    self.ds_matrix.get(coords)
  }
  pub fn get_mut(&mut self, coords: UnsVec) -> Option<&mut HashMap<BeingGenTemplIdAndFac, SpawnWeight>>{
    self.ds_matrix.get_mut(coords)
  }
  
  pub fn clear_at(&mut self, coords: UnsVec) {
    if let Some(sw_vec) = self.ds_matrix.get_mut(coords){
      sw_vec.clear();
    }
  }

  pub fn len_at(&mut self, coords: UnsVec) -> usize{
    self.ds_matrix[coords].len()
  }
} 