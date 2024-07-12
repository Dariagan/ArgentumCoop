use strum::EnumCount;

use crate::utils::matrix::Matrix; 
use std::ops::{Index, IndexMut};
pub use crate::tiling::*;

pub struct WorldMatrix {matrix: Matrix<TileUnidArray>,}
impl WorldMatrix {

  pub fn size(&self) -> UnsVec{
    self.matrix.size()
  }

  pub fn new(size: UnsVec) -> Self {
    Self {
      matrix: Matrix::new_with_element_value(size, TileUnidArray::default()), 
    }
  }
  pub fn non_null_tiles_at_unchk(&self, coords: UnsVec) -> impl Iterator<Item = &TileUnid> {
    self.index(coords).iter().filter(move |&&unid| unid != TileUnid::default())
  }

  pub fn get(&self, coords: UnsVec) -> Option<&TileUnidArray> {self.matrix.get(coords)}
  pub fn get_mut(&mut self, coords: UnsVec) -> Option<&mut TileUnidArray> {self.matrix.get_mut(coords)}
  pub unsafe fn count_at(&self, coords: UnsVec) -> usize {
    self[coords]
      .iter()
      .filter(|&&unid| unid != TileUnid::default())
      .count()
  } 
  pub unsafe fn is_empty_at_unchk(&self, coords: UnsVec) -> bool {
    self.count_at(coords) == 0
  }
  pub unsafe fn has_tiles_at_unchk(&self, coords: UnsVec) -> bool {
    self.count_at(coords) > 0
  }
  pub fn is_empty_at() -> Result<bool, ()> {
    todo!()
  }
  pub fn has_tiles_at() -> Result<bool, ()> {
    todo!()
  }

  pub unsafe fn overwrite_tile(&mut self, tile: TileUnid, coords: UnsVec, z_level: TileZLevel){
    let prev_tile = self.get_mut(coords).expect("TODO CAMBIAR POR CORCHETES").arr.get_unchecked_mut(z_level as usize);
    *prev_tile = tile;
  }
  pub unsafe fn place_tile(&mut self, tile: TileUnid, coords: UnsVec, z_level: TileZLevel) -> Result<(), String>{
    let prev_tile = self[coords].arr.get_unchecked_mut(z_level as usize);
    match *prev_tile {
      TileUnid::NULL => {*prev_tile = tile; Ok(())},
      _ => Err("A tile already exists at the specified position".to_string())
    }
  }

}

impl Index<UnsVec> for WorldMatrix {
  type Output = TileUnidArray;
  fn index(&self, coords: UnsVec) -> &TileUnidArray {
    &(self.matrix[coords])
  }
}
impl IndexMut<UnsVec> for WorldMatrix {
  fn index_mut(&mut self, coords: UnsVec) -> &mut TileUnidArray {
    &mut(self.matrix[coords])
  }
}

#[derive(Default, Clone, Copy)]
pub struct TileUnidArray{
  pub arr: [TileUnid; TileZLevel::COUNT]
}
impl TileUnidArray {
  #[inline]
  pub fn iter(&self) -> std::slice::Iter<'_, TileUnid> {
    self.arr.iter()
  }
  #[inline]
  pub fn assign_unid(&mut self, (unid, z_level): (TileUnid, TileZLevel)){
    unsafe{*self.arr.get_unchecked_mut(z_level as usize) = unid;}
  }
}
