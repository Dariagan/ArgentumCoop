use std::hash::{Hash, Hasher};
use godot::prelude::*;
use gxhash::*;
use crate::world_matrix::TileTypeUid;

#[derive(GodotClass)]
#[class(tool, init, base=Resource)]
pub struct Tile {
    base: Base<Resource>,
    #[export]
    id: StringName,
    #[export]
    layer: TileZLevel,
    #[export]
    source_atlas: i64,
    #[export]
    origin_position: Vector2i,
    #[export]
    modulo_tiling_area: Vector2i,
    #[export]
    alternative_id: i64,

    pub assigned_uid: TileTypeUid,
}

impl Tile {
    pub fn base(&self) -> &Base<Resource> { &self.base }
    pub fn id(&self) -> &StringName { &self.id }
    pub fn layer(&self) -> i64 { self.layer }
    pub fn source_atlas(&self) -> i64 { self.source_atlas }
    pub fn origin_position(&self) -> Vector2i { self.origin_position }
    pub fn modulo_tiling_area(&self) -> Vector2i { self.modulo_tiling_area }
    pub fn alternative_id(&self) -> i64 { self.alternative_id }
}

impl Hash for Tile {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.layer.hash(state);
        self.source_atlas.hash(state);
        self.origin_position.hash(state);
        self.modulo_tiling_area.hash(state);
        self.alternative_id.hash(state);
        self.assigned_uid.hash(state);
    }
}