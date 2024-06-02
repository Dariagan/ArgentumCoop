
use godot::prelude::*; use std::hash::{Hash, Hasher};

#[derive(Clone, PartialEq, Copy)]
pub struct TileTypeUid(pub u16);
pub const NULL_TILE: TileTypeUid = TileTypeUid(u16::MAX);

impl Default for TileTypeUid {fn default() -> Self {NULL_TILE}}
impl Hash for TileTypeUid {fn hash<H: Hasher>(&self, state: &mut H) {self.0.hash(state);}}

#[derive(GodotConvert, Var, Export, Clone, Copy)]
#[godot(via = i64)]
pub enum TileZLevel {
    Soil = 0, Floor, Stain, Structure, Roof,
}
impl Default for TileZLevel {fn default() -> Self {Self::Soil}}
impl Hash for TileZLevel {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i8(*self as i8)}}


#[derive(GodotClass)]
#[class(tool, init, base=Resource)]
pub struct Tile {
    base: Base<Resource>,
    #[export] id: StringName,
    #[export] layer: TileZLevel,
    #[export] source_atlas: i64,
    #[export] origin_position: Vector2i,
    #[export] modulo_tiling_area: Vector2i,
    #[export] alternative_id: i64,
    #[export] random_scale_range: Vector4,// tal vez es mejor volver a los bushes y trees escenas para poder hacer esto
    #[export] flipped_at_random: bool,
    
    pub assigned_uid: TileTypeUid,
}
impl Tile {
    pub fn base(&self) -> &Base<Resource> { &self.base }
    pub fn id(&self) -> &StringName { &self.id }
    pub fn layer(&self) -> TileZLevel { self.layer }
    pub fn source_atlas(&self) -> i64 { self.source_atlas }
    pub fn origin_position(&self) -> Vector2i { self.origin_position }
    pub fn modulo_tiling_area(&self) -> Vector2i { self.modulo_tiling_area }
    pub fn alternative_id(&self) -> i64 { self.alternative_id }
    pub fn random_scale_range(&self) -> Vector4 { self.random_scale_range }
    pub fn flipped_at_random(&self) -> bool { self.flipped_at_random }
}

impl Hash for Tile {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.layer.hash(state);
        self.source_atlas.hash(state);
        self.origin_position.hash(state);
        self.modulo_tiling_area.hash(state);
        self.alternative_id.hash(state);
    }
}

