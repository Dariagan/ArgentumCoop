use std::ops::{Index, IndexMut};

pub use crate::utils::uns_vec::UnsVec;

pub struct Matrix<T: Default + Clone> {
  flattened_matrix: Vec<T>,
  size: UnsVec,
}
#[allow(dead_code)]
impl<T: Default + Clone> Matrix<T> {
  pub fn size(&self) -> UnsVec {self.size}

  pub fn new(size: UnsVec) -> Self {
    let area: usize = size.area();
    let flattened_matrix = create_reserved_vec::<T>(area);
    Self {
      flattened_matrix, size,
    }
  }
  pub fn new_with_element_value(size: UnsVec, element_value: T) -> Self {
    let area: usize = size.area();
    Self {
      flattened_matrix: vec![element_value; area],
      size,
    }
  }
  pub fn get(&self, coords: UnsVec) -> Option<&T> {
    self.flattened_matrix.get(coords.flat_index(&self.size))
  }
  pub fn get_mut(&mut self, coords: UnsVec) -> Option<&mut T> {
    self.flattened_matrix.get_mut(coords.flat_index(&self.size))
  }
}
impl<T: Default + Clone> Index<UnsVec> for Matrix<T> {
  type Output = T;
  fn index(&self, coords: UnsVec) -> &T {
    unsafe{self.flattened_matrix.get_unchecked(coords.flat_index(&self.size))}
  }
}
impl<T: Default + Clone> IndexMut<UnsVec> for Matrix<T> {
  fn index_mut(&mut self, coords: UnsVec) -> &mut T {
    unsafe{self.flattened_matrix.get_unchecked_mut(coords.flat_index(&self.size))}
  }
}
pub struct DownScalingMatrix<T: Default> {
  downscale_factor: u8,
  flattened_matrix: Vec<T>,
  downscaled_size: UnsVec,
}
#[allow(dead_code)]
impl<T: Default> DownScalingMatrix<T> {
  pub fn size(&self) -> UnsVec {self.downscaled_size}
  pub fn new(size_to_downscale_from: UnsVec, downscale_factor: u8) -> Self {
    let downscale_factor = downscale_factor.max(1);

    let downscaled_size = size_to_downscale_from / downscale_factor;
    let area: usize = downscaled_size.area();
    let flattened_matrix = create_reserved_vec::<T>(area);
    Self {
      downscale_factor,
      flattened_matrix,
      downscaled_size,
    }
  }
  pub fn get_unchecked(&self, coords: UnsVec) -> &T {
    unsafe{
    let downscaled_coords = coords / self.downscale_factor;
    let i = downscaled_coords.flat_index(&self.downscaled_size);
    return self.flattened_matrix.get_unchecked(i);}
  }
  pub fn get_unchecked_mut(&mut self, coords: UnsVec) -> &mut T {
    unsafe{
    let downscaled_coords = coords / self.downscale_factor;
    let i = downscaled_coords.flat_index(&self.downscaled_size);
    return self.flattened_matrix.get_unchecked_mut(i);}
  }
  pub fn get(&self, coords: UnsVec) -> Option<&T> {
    let downscaled_coords = coords / self.downscale_factor;
    let i = downscaled_coords.flat_index(&self.downscaled_size);
    return self.flattened_matrix.get(i);
  }
  pub fn get_mut(&mut self, coords: UnsVec) -> Option<&mut T> {
    let downscaled_coords = coords / self.downscale_factor;
    let i = downscaled_coords.flat_index(&self.downscaled_size);
    return self.flattened_matrix.get_mut(i);
  }
}
impl<T: Default> Index<UnsVec> for DownScalingMatrix<T> {
  type Output = T;
  fn index(&self, coords: UnsVec) -> &T {
    self.get_unchecked(coords)
  }
}
impl<T: Default> IndexMut<UnsVec> for DownScalingMatrix<T> {
  fn index_mut(&mut self, coords: UnsVec) -> &mut T {
    self.get_unchecked_mut(coords)
  }
}

fn create_reserved_vec<T: Default>(area: usize) -> Vec<T> {
  let mut flattened_matrix: Vec<T> = Vec::new();
  flattened_matrix.reserve_exact(area);
  flattened_matrix.resize_with(area, T::default);
  flattened_matrix
}