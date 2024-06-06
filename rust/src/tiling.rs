
use godot::{register::GodotClass, prelude::*}; use std::hash::{Hash, Hasher};

use crate::formation_generator::NidOrNidDistribution;

#[derive(Clone, PartialEq, Copy)]
pub struct TileTypeNid(pub u16);
pub const NULL_TILE: TileTypeNid = TileTypeNid(u16::MAX);

impl Default for TileTypeNid {fn default() -> Self {NULL_TILE}}
impl Hash for TileTypeNid {fn hash<H: Hasher>(&self, state: &mut H) {self.0.hash(state);}}

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
    
    pub nid: Option<TileTypeNid>,
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

#[derive(GodotClass)]
#[class(tool, init, base=Resource)]
pub struct TileSelection {
    base: Base<Resource>,
    #[export] id: StringName,
    #[export] targets: Array<StringName>,
    #[export] use_distribution: Array<bool>,
    #[export] tiles: Array<Gd<Tile>>,
    #[export] tiles_distributions: Array<Gd<TileDistribution>>,

}
#[godot_api]
impl TileSelection {
    pub fn id(&self) -> &StringName { &self.id }
    pub fn targets(&self) -> &Array<StringName> {&self.targets}
    
    pub fn use_distribution(&self) -> &Array<bool>{&self.use_distribution}
    pub fn tiles(&self) -> &Array<Gd<Tile>> {&self.tiles}
    pub fn tiles_distributions(&self) -> &Array<Gd<TileDistribution>> {&self.tiles_distributions}

    

    #[func]
    pub fn is_valid(&self) -> bool {
        self.tiles_distributions().len() == self.targets().len()
    }
}

#[derive(GodotClass)]
#[class(tool, init, base=Resource)]
pub struct TileDistribution {
    base: Base<Resource>,
    #[export] id: StringName,
    #[export] tiles: Array<Gd<Tile>>,
    #[export] weights: Array<i64>,

}
#[godot_api]
impl TileDistribution {
    pub fn base(&self) -> &Base<Resource> { &self.base }
    pub fn id(&self) -> &StringName { &self.id }

    #[func]
    pub fn is_valid(&self) -> bool {
        ! self.tiles.is_empty()
        && self.tiles.len() == self.weights.len() 
        && self.tiles.iter_shared().all(|tile| tile.bind().nid.is_some())
        && self.weights.iter_shared().all(|x| x >= 0) 
    }
}
#[derive(Debug)]
pub enum TileDistributionError {
    InvalidLength,
    NegativeWeight,
    MissingNid
}
impl std::fmt::Display for TileDistributionError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            TileDistributionError::InvalidLength => write!(f, "Tiles and weights arrays must have the same length and contain at least one element."),
            TileDistributionError::NegativeWeight => write!(f, "Weights array contains negative values."),
            TileDistributionError::MissingNid => write!(f, "Nid is missing for a tile(s)"),
        }
    }
}
impl TryInto<NidOrNidDistribution> for TileDistribution {
    type Error = TileDistributionError;
    
    fn try_into(self) -> Result<NidOrNidDistribution, Self::Error> {
        if self.tiles.is_empty() || self.tiles.len() != self.weights.len() {
            return Err(TileDistributionError::InvalidLength);
        }
        if self.weights.iter_shared().any(|x| x < 0) {
            return Err(TileDistributionError::NegativeWeight);
        }
        if self.tiles.iter_shared().any(|tile| tile.bind().nid.is_none()){
            return Err(TileDistributionError::MissingNid);
        }
        unsafe{
        if self.tiles.len() == 1 {
            Ok(NidOrNidDistribution::Nid(self.tiles.get(0).unwrap_unchecked().bind().nid.unwrap_unchecked()))
        }
        else{
            let mut distribution = Vec::new();
            distribution.reserve_exact(self.tiles.len());

            for it in self.tiles.iter_shared().zip(self.weights.iter_shared()){
                let (tile, weight) = it;
                distribution.push((tile.bind().nid.unwrap_unchecked(), weight));
            }

            Ok(NidOrNidDistribution::Distribution(distribution))
        }
        }
    }
}

