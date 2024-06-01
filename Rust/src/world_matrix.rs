use crate::matrix::Matrix;
use crate::safevec::SafeVec;
use ranged_num::Ranged;
use typenum::{Unsigned, U0, U5};

#[derive(Clone, PartialEq, Copy)]
pub struct TileTypeUid(u16);
pub const NULL_TILE: TileTypeUid = TileTypeUid(u16::MAX);

impl Default for TileTypeUid {
    fn default() -> Self {
        NULL_TILE
    }
}

type Max_Tiles_Per_Pos = U5;
const MAX_TILES_PER_POS: usize = Max_Tiles_Per_Pos::USIZE;

pub struct WorldMatrix {
    tiles: Matrix<[TileTypeUid; MAX_TILES_PER_POS]>,
}

impl WorldMatrix {
    pub fn new(size: SafeVec) -> Self {
        let initial_value = [TileTypeUid::default(); MAX_TILES_PER_POS];
        Self {
            tiles: Matrix::new_with_initial_value(size, &initial_value), // Assuming Matrix::new takes a SafeVec and an initial value
        }
    }
    pub unsafe fn at_unchk(&self, coords: SafeVec) -> &[TileTypeUid; MAX_TILES_PER_POS as usize] {
        self.tiles.at_unchk_no_ds(coords)
    }
    pub unsafe fn at_unchk_mut(&mut self, coords: SafeVec) -> &mut [TileTypeUid; MAX_TILES_PER_POS] {
        self.tiles.at_unchk_no_ds_mut(coords)
    }

    pub fn at(&self, coords: SafeVec) -> Option<&[TileTypeUid; MAX_TILES_PER_POS as usize]> {
        self.tiles.at_ds(coords)
    }
    pub fn at_mut(&mut self, coords: SafeVec) -> Option<&mut [TileTypeUid; MAX_TILES_PER_POS]> {
        self.tiles.at_mut_ds(coords)
    }
    pub unsafe fn count_at(&self, coords: SafeVec) -> usize {
        self.at_unchk(coords)
            .iter()
            .filter(|&&uid| uid != TileTypeUid::default())
            .count()
    }
    pub unsafe fn is_empty_at_unchk(&self, coords: SafeVec) -> bool {
        self.count_at(coords) == 0
    }
    pub unsafe fn is_not_empty_at_unchk(&self, coords: SafeVec) -> bool {
        !self.is_empty_at_unchk(coords)
    }
    pub unsafe fn overwrite_tile_at_i(&mut self, tile: TileTypeUid, coords: SafeVec, i: Ranged::<U0, Max_Tiles_Per_Pos, usize>){
        let prev_tile = self.at_unchk_mut(coords).get_unchecked_mut(i.value());
        *prev_tile = tile;
    }
    
    pub unsafe fn place_tile_at_i(&mut self, tile: TileTypeUid, coords: SafeVec, i: Ranged::<U0, Max_Tiles_Per_Pos, usize>) -> Result<(), String>{
        let prev_tile = self.at_unchk_mut(coords).get_unchecked_mut(i.value());
        match *prev_tile {
            NULL_TILE => {*prev_tile = tile; Ok(())},
            _ => Err("A tile already exists at the specified position".to_string())
        }
    }

}

