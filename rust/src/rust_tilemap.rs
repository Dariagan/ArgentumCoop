use std::borrow::Borrow;

use crate::formation_generator::{CenteredIterator, FormGenEnum, IFormationGenerator, SizeIterator};
use crate::fractured_formation_generator::FracturedFormationGenerator;
use crate::safe_vec::SafeVec;
use crate::uns_vec::{UnsVec, ZERO};
use crate::{formation_generator, world_matrix::*};
use godot::builtin::Dictionary;
use godot::engine::{ITileMap, TileMap};
use godot::global::print;
use godot::prelude::*;
use num_traits::FromPrimitive;

#[derive(GodotClass)]
#[class(base=TileMap)]
struct RustTileMap {
    #[var]
    seed: i64,
    #[var]
    tile_nid_mapping: Array<Gd<Tile>>,
    world_matrix: Option<WorldMatrix>,
    base: Base<TileMap>,
    world_size: UnsVec,
}
#[godot_api]
impl ITileMap for RustTileMap {
    fn init(base: Base<TileMap>) -> Self {
        Self {
            base,
            seed: 0,
            world_matrix: None,
            tile_nid_mapping: Array::new(),
            world_size: ZERO
        }
    }
}

#[godot_api]
impl RustTileMap {
    #[func]
    fn generate_world_matrix(&mut self, size: Vector2i, tiles: Array<Gd<Tile>>) {
        
        assert!(tiles.len() < NULL_TILE.0 as usize);
        
        self.tile_nid_mapping.extend(tiles.iter_shared()
            .enumerate() 
            .map(|(i, mut tile)| {
                tile.bind_mut().nid = Some(TileTypeNid{0: i as u16}); 
                tile
            })
        );
        let size: UnsVec = size.try_into().unwrap();

        self.world_matrix = Some(WorldMatrix::new(size));     
        self.world_size = size
    }
    #[func]
    fn generate_formation(&mut self, formation: i64, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) -> bool{
        let formation = FormGenEnum::from_i64(formation).unwrap();
        let (origin, size) = (UnsVec::try_from(origin).unwrap(), UnsVec::try_from(size).unwrap()); 

        const MIN_SIZE: u32 = 100;
        if size.all_bigger_than_min(MIN_SIZE).is_err(){
            godot_error!("formation size is too small, must be at least {MIN_SIZE}X{MIN_SIZE}");
            return false;
        }
        if origin.lef + size.lef > self.world_matrix.as_ref().unwrap().size().lef{
            godot_error!("formation would go out of world bounds eastward");
            return false;
        }
        if origin.right + size.right > self.world_matrix.as_ref().unwrap().size().right{
            godot_error!("formation would go out of world bounds southward");
            return false;
        }
        self.world_matrix = Some(
            match formation {
                FormGenEnum::FracturedFormationGenerator => {
                    FracturedFormationGenerator::generate(
                    self.world_matrix.take().unwrap(), origin, size, tile_selection, seed, data)
            }
        });
        true
    }

    #[func]
    fn load_tiles_around(&self, local_coords: Vector2, chunk_size: Vector2i, being_nid: i64) {
        let chunk_size: UnsVec = UnsVec::try_from(chunk_size).unwrap().all_bigger_than_min(10).unwrap();
        
        let being_coords: SafeVec = self.base().local_to_map(local_coords).try_into().unwrap(); let local_coords = ();

        CenteredIterator::new(chunk_size)
            .map(|chunk_coord| chunk_coord + being_coords)
            .filter(|coord| coord.is_non_negative() && coord.is_strictly_smaller_than_unsvec(self.world_size))
            .map(Into::into)
            .for_each(|matrix_coord: UnsVec|{

            
            })

    }

    fn set_cell(&mut self, nid: TileTypeNid, coords: UnsVec) {
        let tile: Gd<Tile> = self.get_tile_nid_mapping().at(nid.0 as usize);

        let atlas_origin_position = tile.clone().bind().get_origin_position();
        let modulo_tiling_area = tile.clone().bind().modulo_tiling_area();
        let atlas_origin_position = Vector2i::new(atlas_origin_position.x/modulo_tiling_area.x, atlas_origin_position.y/modulo_tiling_area.y);

        self.base_mut().set_cell_ex(tile.clone().bind().z_level.unwrap() as i32, coords.into())
            .source_id(tile.clone().bind().source_atlas())
            .atlas_coords(atlas_origin_position)
            .alternative_tile(tile.clone().bind().get_alternative_id())
            .done()
    }
}

fn exceeds_tile_limit(arr: &VariantArray) -> Result<(),()> {
    if arr.len() >= NULL_TILE.0 as usize {Ok(())} 
    else {Err(())}
}