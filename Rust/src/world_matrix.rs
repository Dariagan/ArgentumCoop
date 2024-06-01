use std::array;

use crate::safevec::SafeVec;
use crate::matrix::Matrix;

#[derive(Clone, PartialEq, Copy)]
pub struct TileTypeUid(u16);

impl Default for TileTypeUid{
  fn default() -> Self {
    Self(u16::MAX)
  }
}

const MAX_TILES_PER_POS: usize = 5;

pub struct WorldMatrix{
  tiles: Matrix<[TileTypeUid; MAX_TILES_PER_POS]>
}

impl WorldMatrix {

  pub fn new(size: SafeVec) -> Self {
    let initial_value = [TileTypeUid::default(); MAX_TILES_PER_POS];
    Self {
        tiles: Matrix::new_with_initial_value(size, &initial_value),  // Assuming Matrix::new takes a SafeVec and an initial value
    }
  }
  pub fn at(&self, coords: SafeVec) -> &[TileTypeUid; MAX_TILES_PER_POS as usize] {
    &self.tiles[coords]
  }
  pub fn at_mut(&mut self, coords: SafeVec) -> &mut [TileTypeUid; MAX_TILES_PER_POS] {
    &mut self.tiles[coords]
  }
  pub fn count_at(&self, coords: SafeVec) -> usize {
    self.at(coords).iter().filter(|&&uid| uid != TileTypeUid::default()).count()
  }
  pub fn is_empty_at(&self, coords: SafeVec) -> bool {
    self.count_at(coords) == 0
  }
  pub fn is_not_empty_at(&self, coords: SafeVec) -> bool {
    !self.is_empty_at(coords)
  }
}

impl Default for WorldMatrix{
  fn default() -> Self {
    Self(u16::MAX)
  }
}