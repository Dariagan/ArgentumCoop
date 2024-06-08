use crate::formation_generator::{FormGenEnum, IFormationGenerator};
use crate::fractured_formation_generator::FracturedFormationGenerator;
use crate::uns_vec::UnsVec;
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
    
}
#[godot_api]
impl ITileMap for RustTileMap {
    fn init(base: Base<TileMap>) -> Self {
        Self {
            base,
            seed: 0,
            world_matrix: None,
            tile_nid_mapping: Array::new()
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

        self.world_matrix = Some(WorldMatrix::new(size.try_into().unwrap()));     
    }
    #[func]
    fn generate_formation(&mut self, formation: i64, origin: Vector2i, size: Vector2i, tile_selection: Gd<TileSelection>, seed: i32, data: Dictionary) {
        let formation = FormGenEnum::from_i64(formation).unwrap();
        let (origin, size) = (UnsVec::try_from(origin).unwrap(), UnsVec::try_from(size).unwrap()); 
        
        self.world_matrix = Some(
            match formation {
                FormGenEnum::FracturedFormationGenerator => {
                    FracturedFormationGenerator::generate(
                    self.world_matrix.take().unwrap(), origin, size, tile_selection, seed, data)
            }
        })
    }
   
}

fn exceeds_tile_limit(arr: &VariantArray) -> Result<(),()> {
    if arr.len() >= NULL_TILE.0 as usize {Ok(())} 
    else {Err(())}
}