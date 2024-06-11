use std::borrow::Borrow;

use crate::formation_generator::{FormGenEnum, IFormationGenerator, };
use crate::fractured_continent_generator::FracturedContinentGenerator;
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
        let size: UnsVec = size.try_into().expect("passed arg size: Vector2i is negative");

        const MIN_SIZE: u32 = 500;
        if size.all_bigger_than_min(MIN_SIZE).is_err(){
            godot_error!("world size is too small, must be at least {MIN_SIZE}X{MIN_SIZE}");
        }

        self.world_matrix = Some(WorldMatrix::new(size));     
        self.world_size = size
    }
    #[func]
    fn generate_formation(&mut self, formation: i64, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) -> bool{
        let formation: FormGenEnum = FormGenEnum::from_i64(formation).expect("hola");
        let (origin, size) = 
            (UnsVec::try_from(origin).expect("passed arg origin: Vector2i is negative"), 
            UnsVec::try_from(size).expect("passed arg size: Vector2i is negative")); 

        const MIN_SIZE: u32 = 100;
        if size.all_bigger_than_min(MIN_SIZE).is_err(){
            godot_error!("formation size is too small, must be at least {MIN_SIZE}X{MIN_SIZE}");
            return false;
        }
        if origin.lef + size.lef > self.world_matrix.as_ref().expect("world matrix was not generated before formation").size().lef{
            godot_error!("formation would go out of world bounds eastward");
            return false;
        }
        unsafe{
            if origin.right + size.right > self.world_matrix.as_ref().unwrap_unchecked().size().right{
                godot_error!("formation would go out of world bounds southward");
                return false;
            }
        }
        self.world_matrix = Some(
            match formation {
                FormGenEnum::FracturedContinentGenerator => {
                    FracturedContinentGenerator::generate(
                    self.world_matrix.take().expect("world matrix not present when attempting to generate"), origin, size, tile_selection, seed, data)
            }
        });
        true
    }

    #[func]
    fn load_tiles_around(&mut self, _local_coords: Vector2, chunk_size: Vector2i, being_nid: i64) {
        let chunk_size = UnsVec::try_from(chunk_size).expect("arg chunk_size is negative").all_bigger_than_min(10).expect("chunk size is smaller than minimum 10");
        
        let being_coords: SafeVec = self.base().local_to_map(_local_coords).into(); let _local_coords = ();

        let world_size = self.world_size;
        for chunk_coord in chunk_size.centered_iter()
            .map(|vec| vec + being_coords)
            .filter(|vec| vec.is_non_negative())
            .map(|vec|unsafe{UnsVec::try_from(vec).unwrap_unchecked()})
            .filter(|vec| vec.is_strictly_smaller_than(world_size))
        {
            let tiles = self.world_matrix.as_ref().expect("hola")[chunk_coord];

            tiles.iter().for_each(|nid| self.set_cell(*nid, chunk_coord))
        }

    }

    fn set_cell(&mut self, nid: TileTypeNid, matrix_coord: UnsVec) {
        let tile: Gd<Tile> = self.get_tile_nid_mapping().get(nid.0 as usize).expect(format!("tile mapped to nid={} not found", nid.0).as_str());

        let atlas_origin_position = tile.clone().bind().get_origin_position();
        let modulo_tiling_area = tile.clone().bind().modulo_tiling_area();
        let atlas_origin_position = Vector2i::new(atlas_origin_position.x%modulo_tiling_area.x, atlas_origin_position.y%modulo_tiling_area.y);

        self.base_mut().set_cell_ex(tile.clone().bind().z_level.expect("hola") as i32, matrix_coord.into())
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