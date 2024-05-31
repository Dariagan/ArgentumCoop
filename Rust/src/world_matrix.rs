use std::array;

use crate::safevec::SafeVec;
use crate::matrix::Matrix;

struct TileTypeUid(u16);

const NULL_TILE_UID: TileTypeUid = TileTypeUid(u16::MAX);
const MAX_TILES_PER_POS: u8 = 5;

pub struct WorldMatrix{
    tiles: Matrix<TileTypeUid>
}

const fn initialize_uids_array_as_empty() -> std::array<>{

}