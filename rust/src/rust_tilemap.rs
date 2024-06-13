use std::borrow::Borrow;

use crate::formation_generator::{FormGenEnum, IFormationGenerator, };
use crate::fractured_continent_generator::FracturedContinentGenerator;
use crate::safe_vec::SafeVec;
use crate::uns_vec::{UnsVec, ZERO};
use crate::{formation_generator, world_matrix::{self, *}};
use godot::builtin::Dictionary;
use godot::engine::{ITileMap, TileMap};
use godot::global::print;
use godot::prelude::*;
use num_traits::FromPrimitive;
use rand::{thread_rng, Rng};

#[derive(GodotClass)]
#[class(base=TileMap)]
struct RustTileMap {
    #[var]
    seed: i64,
    tile_nid_mapping: Vec<TileDto>,
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
            tile_nid_mapping: Vec::new(),
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
                tile.bind_mut().unid = Some(TileUnid{0: i as u16}); 
                tile.into()
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
    fn generate_formation(&mut self, formation: FormGenEnum, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) -> bool{
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
        let chunk_size = SafeVec::from(chunk_size).all_bigger_than_min(10).expect("chunk size is smaller than minimum 10");
        
        let being_coords: SafeVec = self.base().local_to_map(_local_coords).into(); let _local_coords = ();

        let world_size = self.world_size;

        //comparar vs for loops comúnes nesteados
        for chunk_coord in (-chunk_size.lef as i32/2..chunk_size.lef as i32/2).flat_map(|i| (-chunk_size.right as i32/2..chunk_size.right as i32/2).map(move |j| (i,j)))
            .map(|vec| SafeVec::from(vec) + being_coords)
            .filter(|vec| vec.is_non_negative())
            .map(|vec|unsafe{UnsVec::try_from(vec).unwrap_unchecked()})
            .filter(|vec| vec.is_strictly_smaller_than(world_size)){
                unsafe{
                    //TODO NO HACER SET CELL SI LA TILE YA ESTÁ CARGADA (CAUSA LAG, HACERLO COMO EN C++)
                    let tiles = self.world_matrix.as_ref().unwrap_unchecked()[chunk_coord];//ojo con esto mientras se genera
                    
                    let non_null_tiles = tiles.iter().filter(|&&unid| unid != TileUnid::default());

                    non_null_tiles.for_each(|&unid| self.set_cell(unid, chunk_coord))
                }
            }
    }

    fn set_cell(&mut self, unid: TileUnid, matrix_coord: UnsVec) {
        let tile: *const TileDto = self.tile_nid_mapping().get(unid.0 as usize).expect("tile mapped to unid= not found");
        unsafe{
        let atlas_origin_position = (*tile).origin_position;
        let modulo_tiling_area = (*tile).modulo_tiling_area;
        let atlas_origin_position_offset = matrix_coord.mod_uns(modulo_tiling_area.try_into().unwrap_unchecked()); 
        let atlas_origin_position = Vector2i::new(atlas_origin_position.x, atlas_origin_position.y);
        self.base_mut().set_cell_ex((*tile).z_level as i32, matrix_coord.into())
            .source_id((*tile).source_atlas)
            .atlas_coords(atlas_origin_position + atlas_origin_position_offset.into())
            .alternative_tile((*tile).alternative_id)
            .done()
    }}

    pub fn tile_nid_mapping(&self) -> &Vec<TileDto> {&self.tile_nid_mapping}
}

fn exceeds_tile_limit(arr: &VariantArray) -> Result<(),()> {
    if arr.len() >= NULL_TILE.0 as usize {Ok(())} 
    else {Err(())}
}