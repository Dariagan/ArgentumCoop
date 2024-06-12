use godot::{register::GodotClass, prelude::*};
use rand_distr::{Distribution, WeightedAliasIndex}; use std::fmt::{self, format};
use std::hash::{Hash, Hasher};
use rand::prelude::*;

use crate::uns_vec::UnsVec;

#[derive(Clone, PartialEq, Copy)]
pub struct TileUnid(pub u16);
pub const NULL_TILE: TileUnid = TileUnid(u16::MAX);

impl Default for TileUnid {fn default() -> Self {NULL_TILE}}
impl Hash for TileUnid {fn hash<H: Hasher>(&self, state: &mut H) {self.0.hash(state);}}

#[derive(GodotConvert, Var, Export, Clone, Copy, EnumCount, Debug, Display)]
#[godot(via = i32)]
pub enum TileZLevel {
    Bottom = 0, Floor, Stain, Structure, Roof,
}

impl Default for TileZLevel {fn default() -> Self {Self::Bottom}}
impl Hash for TileZLevel {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i8(*self as i8)}}

#[derive(GodotClass)]
#[class(tool, base=Resource)]
pub struct Tile {
    base: Base<Resource>,
    #[export] id: StringName,
    #[export] z_level: TileZLevel,
    #[export] source_atlas: i32,
    #[export] origin_position: Vector2i,
    #[export] modulo_tiling_area: Vector2i,
    #[export] alternative_id: i32,
    #[export] random_scale_range: Vector4,// tal vez es mejor volver a los bushes y trees escenas para poder hacer esto
    #[export] flipped_at_random: bool,
    
    pub unid: Option<TileUnid>,
}
#[godot_api]
impl Tile {
    pub fn base(&self) -> &Base<Resource> { &self.base }
    pub fn id(&self) -> &StringName { &self.id }
    pub fn z_level(&self) -> TileZLevel { self.z_level }
    pub fn source_atlas(&self) -> i32 { self.source_atlas }
    pub fn origin_position(&self) -> Vector2i { self.origin_position }
    pub fn modulo_tiling_area(&self) -> Vector2i { self.modulo_tiling_area }
    pub fn alternative_id(&self) -> i32 { self.alternative_id }
    pub fn random_scale_range(&self) -> Vector4 { self.random_scale_range }
    pub fn flipped_at_random(&self) -> bool { self.flipped_at_random }

    #[func]
    fn is_valid(&mut self) -> bool {
        let modulo_tiling_area: UnsVec = self.modulo_tiling_area.try_into().expect(format!("modulo tiling area for Tile id={} must be bigger or equal than (1,1)", self.id()).as_str());
        modulo_tiling_area.all_bigger_than_min(1).expect(format!("modulo tiling area for Tile id={} must be bigger or equal than (1,1)", self.id()).as_str());
        unsafe{assert!(self.id != StringName::try_from("").unwrap(), "id not assigned for Tile id={}", self.id())}

        true
    }
}
#[godot_api]
impl IResource for Tile{
    fn init(base: Base<Resource>) -> Self {
        Self {base, id: StringName::try_from("").expect("stringn"), z_level: TileZLevel::Bottom, source_atlas: -1, origin_position: Vector2i{x: 0, y: 0}, modulo_tiling_area: Vector2i{x: 1, y: 1}, 
        alternative_id: 0, random_scale_range: Vector4{x: 1.0, y: 1.0, z: 1.0, w: 1.0}, flipped_at_random: false, unid: None }
    }
}


impl fmt::Display for TileUnid {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "tunid{}", self.0)
    }
}

// impl Hash for Tile {
//     fn hash<H: Hasher>(&self, state: &mut H) {
//         self.layer.hash(state);
//         self.source_atlas.hash(state);
//         self.origin_position.hash(state);
//         self.modulo_tiling_area.hash(state);
//         self.alternative_id.hash(state);
//     }
// }

#[derive(GodotClass)]
#[class(init, tool, base=Resource)]
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
        self.targets.len() >= self.tiles_distributions().len() &&
        self.targets.len() >= self.tiles().len() &&
        self.tiles_distributions().len() == self.use_distribution().len()
    }
}
//TODO HACERLE UN INIT CON UNA ID MALA PLACEHOLDER ASÍ SE PUEDE VALIDAR^^^

#[derive(GodotClass)]
#[class(base=Node2D)]
struct MyClass{
    base: Base<Node2D>,
    foo_bar: i64,
}

#[godot_api]
impl INode2D for MyClass {
    fn init(base: Base<Node2D>) -> Self {
        Self {base, foo_bar: 32}
    }
}

#[godot_api]
impl MyClass {
    
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
    type Item = NidOrDist;

    fn next(&mut self) -> Option<Self::Item> {
        let tile_selection = self.tile_selection.bind();
        if self.current_index >= tile_selection.targets().len() {
            return None;
        }
        unsafe{
        let result = if tile_selection.use_distribution.get(self.current_index).is_some_and(|x| x) {
            tile_selection.tiles_distributions.get(self.current_index)
                .expect(format!("No tile distribution available for target \"{}\" (i={}) in TileSelection with id={}", tile_selection.targets().get(self.current_index).unwrap_unchecked(), self.current_index, self.tile_selection.bind().id()).as_str())
                .try_into()
        } else {
            tile_selection.tiles.get(self.current_index)
                .expect(format!("No tile available for target \"{}\"(i={}) in TileSelection with id={}", tile_selection.targets().get(self.current_index).unwrap_unchecked(), self.current_index, self.tile_selection.bind().id()).as_str())
                .try_into()
        };
        self.current_index += 1;
        Some(result.unwrap())
        }
    }
}
#[derive(GodotClass)]
#[class(tool, init, base=Resource)]
pub struct TileDistribution {
    base: Base<Resource>,
    #[export] id: StringName,
    #[export] tiles: Array<Gd<Tile>>,
    #[export] weights: PackedInt32Array,
}
#[godot_api]
impl TileDistribution {
    pub fn base(&self) -> &Base<Resource> { &self.base }
    pub fn id(&self) -> &StringName { &self.id }

    #[func]
    pub fn is_valid(&self) -> bool {
        ! self.tiles.is_empty()
        && self.tiles.len() == self.weights.len() 
        //&& self.tiles.iter_shared().all(|tile| tile.bind().layer >= 0 && tile.bind().layer < TileZLevel::COUNT)
        && self.weights.as_slice().iter().all(|x| *x >= 0) 
    }
}
#[derive(Debug)]
pub enum TileDistributionError {
    InvalidLength,//TODO add tile or distribution id to each error-variant (to find culprit more easily)
    NegativeWeight,
    MissingNid,
    MissingBoth,
}
impl std::fmt::Display for TileDistributionError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            TileDistributionError::InvalidLength => write!(f, "Tiles and weights arrays must have the same length and contain at least one element."),
            TileDistributionError::NegativeWeight => write!(f, "Weights array contains negative values."),
            TileDistributionError::MissingNid => write!(f, "unid is missing for a tile(s)"),
            TileDistributionError::MissingBoth => write!(f, "both unid and z-level are missing for a tile(s)"),
        }
    }
}

impl TryFrom<Gd<TileDistribution>> for NidOrDist {
    type Error = TileDistributionError;
    
    fn try_from(value: Gd<TileDistribution>) -> Result<NidOrDist, Self::Error> {
        let gd_tile_dist = value.bind();

        if gd_tile_dist.tiles.len() == 1 {unsafe{
            let tile = gd_tile_dist.tiles.get(0).unwrap_unchecked();
            return Ok(NidOrDist::Nid((tile.clone().bind().unid.expect("unid not assigned to tile"), tile.clone().bind().z_level)));
        }}

        if gd_tile_dist.tiles.is_empty() || gd_tile_dist.tiles.len() != gd_tile_dist.weights.len() {
            return Err(TileDistributionError::InvalidLength);
        }
        if gd_tile_dist.weights.as_slice().iter().any(|x| *x < 0) {
            return Err(TileDistributionError::NegativeWeight);
        }
        if gd_tile_dist.tiles.iter_shared().any(|tile| tile.bind().unid.is_none()){
            return Err(TileDistributionError::MissingNid);
        }
        
        unsafe{
            let mut choices: Vec<(TileUnid, TileZLevel)> = Vec::new();
            let sampler = WeightedAliasIndex::new(gd_tile_dist.weights.as_slice().to_vec()).expect("couldn't create WeightedAliasIndex");
            choices.reserve_exact(gd_tile_dist.tiles.len());

            for tile in gd_tile_dist.tiles.iter_shared(){
                choices.push((tile.bind().unid.expect("unid not assigned to tile"), tile.bind().z_level));
            }

            Ok(NidOrDist::Dist(DiscreteDistribution::new(choices, sampler)))
        }
    }
    
}

impl TryFrom<Gd<Tile>> for NidOrDist {
    type Error = TileDistributionError;
    fn try_from(value: Gd<Tile>) -> Result<NidOrDist, TileDistributionError> {

        let (unid, z_level) = (value.bind().unid, value.bind().z_level);
        match (unid, z_level){
            (Some(unid),z_level) => Ok(NidOrDist::Nid((unid, z_level))),
            (None, _) => Err(TileDistributionError::MissingNid),
        }
    }
}

pub struct DiscreteDistribution{
    choices: Vec<(TileUnid, TileZLevel)>,
    sampler: WeightedAliasIndex<i32>,
}
impl DiscreteDistribution{
    pub fn new(choices: Vec<(TileUnid, TileZLevel)>, sampler: WeightedAliasIndex<i32>,) -> Self {
        Self {choices, sampler}
    }
    pub fn sample(&self) -> (TileUnid, TileZLevel){
        unsafe{
            self.choices.get_unchecked(self.sampler.sample(&mut thread_rng())).clone()
        }
    }
}

pub enum NidOrDist{
    Nid((TileUnid, TileZLevel)),
    Dist(DiscreteDistribution)
}
impl Default for NidOrDist{fn default() -> Self {Self::Nid((TileUnid::default(), TileZLevel::default()))}}

impl NidOrDist{
    pub fn get_a_nid(&self) -> (TileUnid, TileZLevel){
        match self {
            NidOrDist::Nid(x) => *x,
            NidOrDist::Dist(dist) => dist.sample(),
        }
    }
}


pub fn fill_targets(nids_arr: &mut[NidOrDist], target_names: &[&str], tile_selection: Gd<TileSelection>){
    assert_eq!(nids_arr.len(), target_names.len(), "nids arr not equal in length with target_names");
    assert!(target_names.len() <= tile_selection.bind().targets().len(), "TileSelection(id={} len={}) doesn't provide for all target_names(len={})", tile_selection.bind().id, tile_selection.bind().targets().len(), target_names.len());

    GdTileSelectionIterator::new(tile_selection.clone()).zip(tile_selection.bind().targets().iter_shared())
        .for_each(|it: (NidOrDist, StringName)| {
            let (nid_or_distribution, target) = it;

            if let Some(target_i) = target_names.into_iter().position(|name: &&str| *name == target.to_string().as_str()) {
                unsafe{
                    *nids_arr.get_mut(target_i).expect("err fill_targets") = nid_or_distribution
                }
            }
            else {godot_script_error!("target {} not found: ", target);}
        })
}