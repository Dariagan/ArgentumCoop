use crate::matrix::Matrix; pub use crate::uns_vec::UnsVec;
use godot::prelude::*;
use rand::seq::index; use std::{collections::hash_set::Iter, ops::{Index, IndexMut}};
pub use crate::tiling::*;

const MAX_TILES_PER_POS: usize = TileZLevel::Roof as usize + 1;

type TileArray = [TileUnid; MAX_TILES_PER_POS];

pub struct WorldMatrix {tiles: Matrix<TileArray>,}

impl WorldMatrix {

    pub fn size(&self) -> UnsVec{
        self.tiles.size()
    }

    pub fn new(size: UnsVec) -> Self {
        let initial_value = [TileUnid::default(); MAX_TILES_PER_POS];
        Self {
            tiles: Matrix::new_with_element_value(size, &initial_value), 
        }
    }
    pub fn at(&self, coords: UnsVec) -> Option<&TileArray> {self.tiles.at(coords)}
    pub fn at_mut(&mut self, coords: UnsVec) -> Option<&mut TileArray> {self.tiles.at_mut(coords)}
    pub unsafe fn count_at(&self, coords: UnsVec) -> usize {
        self[coords]
            .iter()
            .filter(|&&nid| nid != TileUnid::default())
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
        let prev_tile = self.at_mut(coords).expect("TODO CAMBIAR POR CORCHETES").get_unchecked_mut(z_level as usize);
        *prev_tile = tile;
    }
    pub unsafe fn place_tile(&mut self, tile: TileUnid, coords: UnsVec, z_level: TileZLevel) -> Result<(), String>{
        let prev_tile = self[coords].get_unchecked_mut(z_level as usize);
        match *prev_tile {
            NULL_TILE => {*prev_tile = tile; Ok(())},
            _ => Err("A tile already exists at the specified position".to_string())
        }
    }

}

impl Index<UnsVec> for WorldMatrix {
    type Output = TileArray;
    fn index(&self, coords: UnsVec) -> &TileArray {
        &(self.tiles[coords])
    }
}
impl IndexMut<UnsVec> for WorldMatrix {
    fn index_mut(&mut self, coords: UnsVec) -> &mut TileArray {
        &mut(self.tiles[coords])
    }
}