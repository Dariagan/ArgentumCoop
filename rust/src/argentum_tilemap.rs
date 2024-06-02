use crate::world_matrix::*;
use godot::builtin::Dictionary;
use godot::engine::{ITileMap, TileMap};
use godot::prelude::*;

struct InitializedState {}

#[derive(GodotClass)]
#[class(base=TileMap)]
struct RustTileMap {
    #[var(get = get_tiles_data, set = set_tiles_data)]
    tiles_data: Dictionary,
    #[var]
    seed: i64,
    tiles_uid_mapping: Vec<GString>,
    world_matrix: Option<WorldMatrix>,
    base: Base<TileMap>,
}
#[godot_api]
impl ITileMap for RustTileMap {
    fn init(base: Base<TileMap>) -> Self {
        Self {
            base,
            tiles_data: Dictionary::new(),
            seed: 0,
            world_matrix: None,
            tiles_uid_mapping: vec![]
        }
    }
}
#[godot_api]
impl RustTileMap {
    #[func]
    fn generate_world_matrix(&mut self, size: Vector2i) -> bool {
        false
    }

    #[func]
    fn set_tiles_data(&mut self, dict: Dictionary) {
        self.tiles_data = dict.clone()
    }
    #[func]
    fn get_tiles_data(&self) -> Dictionary {
        self.tiles_data.clone()
    }
}
