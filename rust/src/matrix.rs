use std::ops::{Index, IndexMut};

use crate::safevec::SafeVec;

pub struct Matrix<T: Default + Clone> {
    downscale_factor: u16,
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
            downscale_factor: 1,
            flattened_matrix: vec![T::default(); area],
            size,
            area,
        }
    }

    pub fn new_with_initial_value(size: SafeVec, initial_value: &T) -> Self {
        let area: usize = size.area();
        Self {
            downscale_factor: 1,
            flattened_matrix: vec![initial_value.clone(); area],
            size: size,
            area,
        }
    }

    pub fn new_with_downscale(original_size: SafeVec, downscale_factor: u16) -> Self {
        let downscale_factor = downscale_factor.max(1);

        if !original_size.is_strictly_positive() {
            panic!("matrix.rs downscaling constructor error");
        }
        let size = original_size / downscale_factor;

        let area = size.area();
        Self {
            downscale_factor,
            flattened_matrix: vec![T::default(); area],
            size,
            area,
        }
    }
    
    pub unsafe fn at_unchk_no_ds(&self, coords: SafeVec) -> &T {
        self.flattened_matrix.get_unchecked(coords.flat_index(&self.size))
    }
    pub unsafe fn at_unchk_no_ds_mut(&mut self, coords: SafeVec) -> &mut T {
        self.flattened_matrix.get_unchecked_mut(coords.flat_index(&self.size))
    }

    pub fn at_ds(&self, coords: SafeVec) -> Option<&T> {
        let downscaled_coords = coords / self.downscale_factor;
        let i = downscaled_coords.flat_index(&self.size);
        if i < self.flattened_matrix.len() {
            Some(&self.flattened_matrix[i])
        } else {None}
    }
    pub fn at_mut_ds(&mut self, coords: SafeVec) -> Option<&mut T> {
        let downscaled_coords = coords / self.downscale_factor;
        let i = downscaled_coords.flat_index(&self.size);
        if i < self.flattened_matrix.len() {
            Some(&mut self.flattened_matrix[i])
        } else {None}
    }
    pub fn get_size(&self) -> SafeVec {self.size}
    pub fn get_area(&self) -> usize {self.area}
}