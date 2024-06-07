
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
    #[export] use_distribution: Array<bool>,//false:then Tile, true: then TileDistribution
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

pub struct GdTileSelectionIterator{
    tile_selection: Gd<TileSelection>, current_index: usize
}

impl GdTileSelectionIterator{
    pub fn new(tile_selection: Gd<TileSelection>) -> Self {
        Self{tile_selection, current_index: 0}
    }
}

impl Iterator for GdTileSelectionIterator {
    type Item = NidOrNidDistribution;

    fn next(&mut self) -> Option<Self::Item> {
        let val = self.tile_selection.bind();
        if self.current_index >= val.use_distribution.len() {
            return None;
        }

        let result = if val.use_distribution.get(self.current_index).is_some_and(|x| x) {
            val.tiles_distributions.at(self.current_index).try_into()
        } else {
            val.tiles.at(self.current_index).try_into()
        };

        self.current_index += 1;
        Some(result.unwrap())
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

impl TryFrom<Gd<TileDistribution>> for NidOrNidDistribution {
    type Error = TileDistributionError;
    
    fn try_from(value: Gd<TileDistribution>) -> Result<NidOrNidDistribution, Self::Error> {
        let value = value.bind();
        if value.tiles.is_empty() || value.tiles.len() != value.weights.len() {
            return Err(TileDistributionError::InvalidLength);
        }
        if value.weights.iter_shared().any(|x| x < 0) {
            return Err(TileDistributionError::NegativeWeight);
        }
        if value.tiles.iter_shared().any(|tile| tile.bind().nid.is_none()){
            return Err(TileDistributionError::MissingNid);
        }
        unsafe{
        if value.tiles.len() == 1 {
            Ok(NidOrNidDistribution::Nid(value.tiles.get(0).unwrap_unchecked().bind().nid.unwrap_unchecked()))
        }
        else{
            let mut distribution = Vec::new();
            distribution.reserve_exact(value.tiles.len());

            for it in value.tiles.iter_shared().zip(value.weights.iter_shared()){
                let (tile, weight) = it;
                distribution.push((tile.bind().nid.unwrap_unchecked(), weight));
            }

            Ok(NidOrNidDistribution::Distribution(distribution))
        }
        }
    }
    
}

impl TryFrom<Gd<Tile>> for NidOrNidDistribution {
    type Error = TileDistributionError;
    fn try_from(value: Gd<Tile>) -> Result<NidOrNidDistribution, TileDistributionError> {

        match value.bind().nid{
            Some(nid) => Ok(NidOrNidDistribution::Nid(nid)),
            None => Err(TileDistributionError::MissingNid),
        }
    }
    
    
}
