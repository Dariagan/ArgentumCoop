use std::ops::{Index, IndexMut};

use crate::safevec::SafeVec;

pub struct Matrix<T: Default + Clone> {
    flattened_matrix: Vec<T>,
    size: SafeVec,
    area: usize,
}
impl<T: Default + Clone> Matrix<T> {
    pub fn new(size: SafeVec) -> Self {
        if !size.is_strictly_positive() {
            panic!("matrix.rs constructor error");
        }
        let area: usize = size.area();
        Self {
            flattened_matrix: vec![T::default(); area],
            size,
            area,
        }
    }
    pub fn new_with_initial_value(size: SafeVec, initial_value: &T) -> Self {
        let area: usize = size.area();
        Self {
            flattened_matrix: vec![initial_value.clone(); area],
            size: size,
            area,
        }
    }
    
    pub unsafe fn at_unchk_no_ds(&self, coords: SafeVec) -> &T {
        self.flattened_matrix.get_unchecked(coords.flat_index(&self.size))
    }
    pub unsafe fn at_unchk_no_ds_mut(&mut self, coords: SafeVec) -> &mut T {
        self.flattened_matrix.get_unchecked_mut(coords.flat_index(&self.size))
    }

    
    pub fn size(&self) -> SafeVec {self.size}
    pub fn area(&self) -> usize {self.area}
}
impl<T: Default + Clone> Index<SafeVec> for Matrix<T> {
    type Output = T;

    fn index(&self, coords: SafeVec) -> &T {
        unsafe{
            self.flattened_matrix.get_unchecked(coords.flat_index(&self.size))
        }
    }
}
impl<T: Default + Clone> IndexMut<SafeVec> for Matrix<T> {
    fn index_mut(&mut self, coords: SafeVec) -> &mut T {
        unsafe{
            self.flattened_matrix.get_unchecked_mut(coords.flat_index(&self.size))
        }
    }
}

pub struct DownScalingMatrix<T: Default + Clone> {
    downscale_factor: u8,
    flattened_matrix: Vec<T>,
    real_size: SafeVec,
    area: usize,
}
impl<T: Default + Clone> DownScalingMatrix<T> {

    pub fn new_with_initial_value(real_size: SafeVec, downscale_factor: u8, initial_value: &T) -> Self {
        let area: usize = size.area();
        Self {
            flattened_matrix: vec![initial_value.clone(); area],
            real_size,
            area,
            downscale_factor,
        }
    }

    pub fn new(real_size: SafeVec, downscale_factor: u8) -> Self {
        let downscale_factor = downscale_factor.max(1);

        if !real_size.is_strictly_positive() {
            panic!("matrix.rs downscaling constructor error");
        }
        let size = real_size / downscale_factor;

        let area = size.area();
        Self {
            flattened_matrix: vec![T::default(); area],
            size,
            area,
            downscale_factor,
        }
    }
    
    pub unsafe fn at_unchk_no_ds(&self, coords: SafeVec) -> &T {
        self.flattened_matrix.get_unchecked(coords.flat_index(&self.size))
    }
    pub unsafe fn at_unchk_no_ds_mut(&mut self, coords: SafeVec) -> &mut T {
        self.flattened_matrix.get_unchecked_mut(coords.flat_index(&self.size))
    }

    
    pub fn size(&self) -> SafeVec {self.size}
    pub fn area(&self) -> usize {self.area}
}
impl<T: Default + Clone> Index<SafeVec> for DownScalingMatrix<T> {
    type Output = T;

    fn index(&self, coords: SafeVec) -> &T {
        unsafe{
            self.flattened_matrix.get_unchecked(coords.flat_index(&self.size))
        }
    }
}
impl<T: Default + Clone> IndexMut<SafeVec> for DownScalingMatrix<T> {
    fn index_mut(&mut self, coords: SafeVec) -> &mut T {
        unsafe{
            self.flattened_matrix.get_unchecked_mut(coords.flat_index(&self.size))
        }
    }
}


pub fn at(&self, coords: SafeVec) -> Option<&T> {
    let downscaled_coords = coords / self.downscale_factor;
    let i = downscaled_coords.flat_index(&self.size);
    if i < self.flattened_matrix.len() {
        Some(&self.flattened_matrix[i])
    } else {None}
}
pub fn at_mut(&mut self, coords: SafeVec) -> Option<&mut T> {
    let downscaled_coords = coords / self.downscale_factor;
    let i = downscaled_coords.flat_index(&self.size);
    if i < self.flattened_matrix.len() {
        Some(&mut self.flattened_matrix[i])
    } else {None}
}