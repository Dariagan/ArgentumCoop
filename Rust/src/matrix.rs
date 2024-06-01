use std::ops::{Index, IndexMut};

use crate::safevec::SafeVec;

pub struct Matrix<T> {
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

        let area: usize = size.area_usize();
        Self {
            downscale_factor: 1,
            flattened_matrix: vec![T::default(); area],
            size,
            area,
        }
    }

    pub fn new_with_initial_value(size: SafeVec, initial_value: &T) -> Self {
        let area: usize = size.area_usize();
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

        let area = size.area_usize();
        Self {
            downscale_factor,
            flattened_matrix: vec![T::default(); area],
            size,
            area,
        }
    }

    pub fn index_no_downscale(&self, coords: SafeVec) -> &T {
        &self.flattened_matrix[coords.flat_index(&self.size)]
    }

    pub fn index_no_downscale_mut(&mut self, coords: SafeVec) -> &mut T {
        &mut self.flattened_matrix[coords.flat_index(&self.size)]
    }

    pub fn at(&self, coords: SafeVec) -> Option<&T> {
        let downscaled_coords = coords / self.downscale_factor;
        let i = downscaled_coords.flat_index(&self.size);
        if i < self.flattened_matrix.len() {
            Some(&self.flattened_matrix[i])
        } else {
            None
        }
    }

    pub fn at_mut(&mut self, coords: SafeVec) -> Option<&mut T> {
        let downscaled_coords = coords / self.downscale_factor;
        let i = downscaled_coords.flat_index(&self.size);
        if i < self.flattened_matrix.len() {
            Some(&mut self.flattened_matrix[i])
        } else {
            None
        }
    }

    pub fn get_size(&self) -> SafeVec {
        self.size
    }

    pub fn get_area(&self) -> usize {
        self.area
    }
}

impl<T> Index<SafeVec> for Matrix<T> {
    type Output = T;

    fn index(&self, coords: SafeVec) -> &Self::Output {
        let downscaled_coords = SafeVec {
            lef: coords.lef / self.downscale_factor as i32,
            right: coords.right / self.downscale_factor as i32,
        };
        &self.flattened_matrix[downscaled_coords.flat_index(&self.size)]
    }
}

impl<T> IndexMut<SafeVec> for Matrix<T> {
    fn index_mut(&mut self, coords: SafeVec) -> &mut Self::Output {
        let downscaled_coords = SafeVec {
            lef: coords.lef / self.downscale_factor as i32,
            right: coords.right / self.downscale_factor as i32,
        };
        &mut self.flattened_matrix[downscaled_coords.flat_index(&self.size)]
    }
}
