use std::{collections::HashMap, ops::Add};

use crate::utils::{matrix::DownScalingMatrix, uns_vec::UnsVec};

use super::BeingKindStrId;

#[allow(dead_code)]
#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct SpawnWeight(u16);
impl Add for SpawnWeight {
  type Output = SpawnWeight;
  fn add(self, other: Self) -> Self::Output {
    SpawnWeight(self.0 + other.0)
  }
}

pub struct SpawnWeightsMatrix{
  ds_matrix: DownScalingMatrix<HashMap<BeingKindStrId, SpawnWeight>>
}
#[allow(dead_code)]
impl SpawnWeightsMatrix{

  pub fn overwrite_at(&mut self, coords: UnsVec, being_kind_id: BeingKindStrId, new_weight: SpawnWeight) {
    self.ds_matrix.get_unchecked_mut(coords).insert(being_kind_id, new_weight);
  }
  pub fn increase_at(&mut self, coords: UnsVec, being_kind_id: BeingKindStrId, added_weight: SpawnWeight) {
    let hash_map = self.ds_matrix.get_unchecked_mut(coords);
    hash_map.entry(being_kind_id)
      .and_modify(|existing_weight| *existing_weight = *existing_weight + added_weight)
      .or_insert(added_weight);
  }
  
  pub fn cleat_at(&mut self, coords: UnsVec) {
    if let Some(sw_vec) = self.ds_matrix.get_mut(coords){
      sw_vec.clear();
    }
  }

  pub fn len_at(&mut self, coords: UnsVec) -> usize{
    self.ds_matrix[coords].len()
  }
} 