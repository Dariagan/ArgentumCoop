use crate::matrix::Matrix; pub use crate::safe_vec::SafeVec;
use godot::prelude::*; use std::ops::{Index, IndexMut};
pub use crate::tiling::*;

const MAX_TILES_PER_POS: usize = TileZLevel::Roof as usize + 1;

type TileArray = [TileTypeNid; MAX_TILES_PER_POS];

pub struct WorldMatrix {tiles: Matrix<TileArray>,}

impl WorldMatrix {

    pub fn new(size: SafeVec) -> Self {
        let initial_value = [TileTypeNid::default(); MAX_TILES_PER_POS];
        if size.is_any_comp_negative() {
            panic!("attempt to build matrix with negative matrix")}
        Self {
            tiles: Matrix::new_with_element_value(size, &initial_value), 
        }
    }
    pub fn at(&self, coords: SafeVec) -> Option<&TileArray> {self.tiles.at(coords)}
    pub fn at_mut(&mut self, coords: SafeVec) -> Option<&mut TileArray> {self.tiles.at_mut(coords)}
    pub unsafe fn count_at(&self, coords: SafeVec) -> usize {
        self[coords]
            .iter()
            .filter(|&&nid| nid != TileTypeNid::default())
            .count()
    }
    pub unsafe fn is_empty_at_unchk(&self, coords: SafeVec) -> bool {
        self.count_at(coords) == 0
    }
    pub unsafe fn has_tiles_at_unchk(&self, coords: SafeVec) -> bool {
        self.count_at(coords) > 0
    }
    pub fn is_empty_at() -> Result<bool, ()> {
        todo!()
    }
    pub fn has_tiles_at() -> Result<bool, ()> {
        todo!()
    }

    pub unsafe fn overwrite_tile_at_i(&mut self, tile: TileTypeNid, coords: SafeVec, z_level: TileZLevel){
        let prev_tile = self[coords].get_unchecked_mut(z_level as usize);
        *prev_tile = tile;
    }
    pub unsafe fn place_tile_at_i(&mut self, tile: TileTypeNid, coords: SafeVec, z_level: TileZLevel) -> Result<(), String>{
        let prev_tile = self[coords].get_unchecked_mut(z_level as usize);
        match *prev_tile {
            NULL_TILE => {*prev_tile = tile; Ok(())},
            _ => Err("A tile already exists at the specified position".to_string())
        }
    }

}

impl Index<SafeVec> for WorldMatrix {
    type Output = TileArray;
    fn index(&self, coords: SafeVec) -> &TileArray {
        &(self.tiles[coords])
    }
}
impl IndexMut<SafeVec> for WorldMatrix {
    fn index_mut(&mut self, coords: SafeVec) -> &mut TileArray {
        &mut(self.tiles[coords])
    }
}