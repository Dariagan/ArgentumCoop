use godot::engine::{Material, Shader, ShaderMaterial};
use godot::{register::GodotClass, prelude::*};
use rand_distr::{Distribution, WeightedAliasIndex};
use rand_pcg::Lcg128Xsl64; use std::fmt::{self, format};
use std::hash::{Hash, Hasher};

pub use crate::utils::uns_vec::UnsVec;

#[derive(Clone, PartialEq, Copy, Debug)]
pub struct TileUnid(pub u16);
impl TileUnid{pub const NULL: TileUnid = TileUnid(u16::MAX);}
impl Default for TileUnid {fn default() -> Self {TileUnid::NULL}}
impl Hash for TileUnid {fn hash<H: Hasher>(&self, state: &mut H) {state.write_u16(self.0);}}
impl fmt::Display for TileUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Tunid{}", self.0)}}//alt+z

use strum_macros::EnumIter;
#[derive(GodotConvert, Var, Export, Clone, Copy, EnumCount, Debug, Display, EnumIter, VariantNames)]
#[godot(via = i32)]
pub enum TileZLevel {Soil = 0, Floor, Stain, Structure, Roof,}

impl Default for TileZLevel {fn default() -> Self {Self::Soil}} impl Hash for TileZLevel {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i8(*self as i8)}}

#[derive(GodotClass, Debug)]
#[class(tool, base=Resource)]
pub struct Tile {
  base: Base<Resource>,
  #[var] id: StringName,
  #[export] z_level: TileZLevel,
  #[export] source_atlas: i32,
  #[export] origin_position: Vector2i,
  #[export] modulo_tiling_area: Vector2i,
  #[export] alternative_id: i32,
  #[export] random_scale_range: Vector4,// tal vez es mejor volver a los bushes y trees escenas para poder hacer esto
  #[export] flipped_at_random: bool,

  #[export] shader_id: StringName,
  
  pub unid: Option<TileUnid>,
}
#[godot_api]
impl Tile {//TODO hacer
  pub fn base(&self) -> &Base<Resource> { &self.base }
  pub fn id(&self) -> &StringName { &self.id }
  pub fn z_level(&self) -> TileZLevel { self.z_level }
  pub fn source_atlas(&self) -> i32 { self.source_atlas }
  pub fn origin_position(&self) -> Vector2i { self.origin_position }
  pub fn modulo_tiling_area(&self) -> Vector2i { self.modulo_tiling_area }
  pub fn alternative_id(&self) -> i32 { self.alternative_id }
  pub fn random_scale_range(&self) -> Vector4 { self.random_scale_range }
  pub fn flipped_at_random(&self) -> bool { self.flipped_at_random }

  pub fn shader_id(&self) -> &StringName { &self.shader_id }

  #[func]
  fn validate(&self) -> bool {
    let err_msg = format!("modulo tiling area for Tile id={} must be bigger or equal than (1,1)", self.id());
    let modulo_tiling_area: UnsVec = match self.modulo_tiling_area.try_into() {
        Ok(area) => area,
        Err(_) => {
            godot_error!("{}", err_msg);
            return false;
        },
    };
    if modulo_tiling_area.all_bigger_than_min(1).is_err() {
        godot_error!("{}", err_msg);
        return false;
    }
    true
  }
}
#[godot_api]
impl IResource for Tile{
  fn init(base: Base<Resource>) -> Self {
    Self {base, id: StringName::from(""), z_level: TileZLevel::Soil, source_atlas: -1, origin_position: Vector2i{x: 0, y: 0}, modulo_tiling_area: Vector2i{x: 1, y: 1}, 
    alternative_id: 0, random_scale_range: Vector4{x: 1.0, y: 1.0, z: 1.0, w: 1.0}, flipped_at_random: false, unid: None, shader_id: StringName::from("")}
  }
}
impl Into<TileDto> for Gd<Tile> {fn into(self) -> TileDto {let gd_tile = self.bind(); TileDto { id: gd_tile.id().clone(), z_level: gd_tile.z_level(), source_atlas: gd_tile.source_atlas(), origin_position: gd_tile.origin_position(), modulo_tiling_area: gd_tile.modulo_tiling_area().try_into().expect("error negative"), alternative_id: gd_tile.alternative_id(), random_scale_range: gd_tile.random_scale_range(), flipped_at_random: gd_tile.flipped_at_random(), shader_id: gd_tile.shader_id().clone() }}}
pub struct TileDto{
  pub id: StringName,
  pub z_level: TileZLevel,
  pub source_atlas: i32,
  pub origin_position: Vector2i,
  pub modulo_tiling_area: UnsVec,
  pub alternative_id: i32,
  pub random_scale_range: Vector4,
  pub flipped_at_random: bool,

  pub shader_id: StringName,
}

#[derive(GodotClass)]
#[class(init, tool, base=Resource)]
pub struct TileSelection {
  base: Base<Resource>,
  #[var] id: StringName,
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
  pub fn validate(&self) -> bool {
    self.targets.len() >= self.tiles_distributions().len() &&
    self.targets.len() >= self.tiles().len() &&
    self.tiles_distributions().len() == self.use_distribution().len()
  }
}
//TODO HACERLE UN INIT CON UNA ID MALA PLACEHOLDER ASÍ SE PUEDE VALIDAR^^^

pub struct GdTileSelectionIterator{
  tile_selection: Gd<TileSelection>, current_index: usize
}
impl GdTileSelectionIterator{
  pub fn new(tile_selection: Gd<TileSelection>) -> Self {
    Self{tile_selection, current_index: 0}
  }
}
impl Iterator for GdTileSelectionIterator {
  type Item = UnidOrDist;

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
  #[var] id: StringName,
  #[export] tiles: Array<Gd<Tile>>,
  #[export] weights: PackedInt32Array,
}
#[godot_api]
#[allow(dead_code)]
impl TileDistribution {
  pub fn base(&self) -> &Base<Resource> { &self.base }
  pub fn id(&self) -> &StringName { &self.id }
  #[func]
  pub fn validate(&self) -> bool {//hacer que devuelva un stringname con el error?
    ! self.tiles.is_empty()
    && self.tiles.len() == self.weights.len() 
    //&& self.tiles.iter_shared().all(|tile| tile.bind().layer >= 0 && tile.bind().layer < TileZLevel::COUNT)
    && self.weights.as_slice().iter().all(|x| *x >= 0) 
  }
}
#[derive(Debug)]
enum ErrTileOrDistribution{Tile(StringName),Distribution(StringName),}

#[derive(Debug)]
pub enum TileDistributionError {EmptyTilesArr{id: StringName}, NegativeWeight{id: StringName}, MissingUnid{unid_or_dist: ErrTileOrDistribution}, MissingBoth{id: StringName}, MoreWeightsThanTiles{id: StringName}}//TODO add tile or distribution id to each error-variant (to find culprit more easily)
impl std::fmt::Display for TileDistributionError {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    match self {
      TileDistributionError::EmptyTilesArr{id} => write!(f, "TileDistributon id={id}: Tiles array must not be empty."),
      TileDistributionError::MoreWeightsThanTiles{id} => write!(f, "TileDistributon id={id}: There must not be more weights than tiles."),
      TileDistributionError::NegativeWeight{id} => write!(f, "TileDistributon id={id}: Weights array contains negative values."),
      TileDistributionError::MissingUnid{unid_or_dist} => write!(f, "id=: Unid is missing for a tile(s) or dist"),
      TileDistributionError::MissingBoth{id} => write!(f, "TileDistributon id={id}: Both unid and z-level are missing for a tile(s)."),
    }
  }
}

impl TryFrom<Gd<TileDistribution>> for UnidOrDist {
  type Error = TileDistributionError;
  
  fn try_from(mut val: Gd<TileDistribution>) -> Result<UnidOrDist, Self::Error> {

    let gd_tile_dist = val.bind();

    if gd_tile_dist.tiles.is_empty() {
      return Err(TileDistributionError::EmptyTilesArr{id: gd_tile_dist.id().clone()});
    }
    if gd_tile_dist.weights.len() > gd_tile_dist.tiles.len() {
      return Err(TileDistributionError::MoreWeightsThanTiles{id: gd_tile_dist.id().clone()});
    }
    if gd_tile_dist.weights.is_empty() || gd_tile_dist.weights.as_slice().iter().all(|&w| w <= 0) {
      godot_warn!("TileDistributon id={}: None of the weights has a strictly positive value, setting all values to 1", gd_tile_dist.id());
      let tiles_len: usize = gd_tile_dist.tiles.len();
      let ori_weights_len = gd_tile_dist.weights.len();
      std::mem::drop(gd_tile_dist);
      let mut gd_tile_dist = val.bind_mut();
      if ori_weights_len < tiles_len {
        let difference = tiles_len - ori_weights_len;
        for _ in 0..difference {
          gd_tile_dist.weights.push(1);
        }
      }
      for i in 0..ori_weights_len{
        unsafe{*(gd_tile_dist.weights.as_mut_slice().get_unchecked_mut(i)) = 1}   
      }
    }
    let gd_tile_dist = val.bind();
    if gd_tile_dist.tiles.iter_shared().any(|tile| tile.bind().unid.is_none()){
      return Err(TileDistributionError::MissingUnid{unid_or_dist: ErrTileOrDistribution::Distribution(gd_tile_dist.id().clone())});
    }
    if gd_tile_dist.tiles.len() == 1 {unsafe{
      let tile = gd_tile_dist.tiles.get(0).unwrap_unchecked();
      return Ok(UnidOrDist::Nid((tile.bind().unid.unwrap_unchecked(), tile.bind().z_level)));
    }}
    #[allow(unused_unsafe)]
    unsafe{
      let mut choices: Vec<(TileUnid, TileZLevel)> = Vec::new();
      let sampler: WeightedAliasIndex<i32> = WeightedAliasIndex::new(gd_tile_dist.weights.as_slice().to_vec()).unwrap();
      choices.reserve_exact(gd_tile_dist.tiles.len());

      for tile in gd_tile_dist.tiles.iter_shared(){
        let tile = tile.bind();
        choices.push((tile.unid.expect(&format!("Tile id={}: Unid not assigned", tile.id())), tile.z_level()));
      }

      Ok(UnidOrDist::Dist(DiscreteDistribution::new(choices, sampler)))
    }
  }
  
}

impl TryFrom<Gd<Tile>> for UnidOrDist {
  type Error = TileDistributionError;
  fn try_from(value: Gd<Tile>) -> Result<UnidOrDist, TileDistributionError> {

    let tile = value.bind();
    let (unid, z_level) = (tile.unid, tile.z_level);
    match (unid, z_level){
      (Some(unid),z_level) => Ok(UnidOrDist::Nid((unid, z_level))),
      (None, _) => Err(TileDistributionError::MissingUnid{unid_or_dist: ErrTileOrDistribution::Tile(tile.id.clone())}),
    }
  }
}
pub struct DiscreteDistribution{
  choices: Vec<(TileUnid, TileZLevel)>,
  sampler: WeightedAliasIndex<i32>,
}
impl DiscreteDistribution{
  pub fn new(choices: Vec<(TileUnid, TileZLevel)>, sampler: WeightedAliasIndex<i32>,) -> Self {Self {choices, sampler}}
  pub fn sample(&self, rng: &mut Lcg128Xsl64) -> (TileUnid, TileZLevel){unsafe{self.choices.get_unchecked(self.sampler.sample(rng)).clone()}}
}
pub enum UnidOrDist{
  Nid((TileUnid, TileZLevel)), Dist(DiscreteDistribution)
}
impl UnidOrDist{
  pub fn get_unid(&self, rng: &mut Lcg128Xsl64) -> (TileUnid, TileZLevel){
    match self {
      UnidOrDist::Nid(x) => *x,
      UnidOrDist::Dist(dist) => dist.sample(rng),
    }
  }
}
impl Default for UnidOrDist{fn default() -> Self {Self::Nid((TileUnid::default(), TileZLevel::default()))}}

pub fn fill_targets(nids_arr: &mut[UnidOrDist], target_names: &[&str], arg_tile_selection: Gd<TileSelection>){
  let tile_selection = arg_tile_selection.bind();
  assert_eq!(nids_arr.len(), target_names.len(), "nids arr not equal in length with target_names");
  assert!(target_names.len() <= tile_selection.targets().len(), "TileSelection(id={} len={}) doesn't provide for all target_names(len={})", tile_selection.id, tile_selection.targets().len(), target_names.len());

  GdTileSelectionIterator::new(arg_tile_selection.clone()).zip(tile_selection.targets().iter_shared())
    .for_each(|it: (UnidOrDist, StringName)| {
      let (nid_or_distribution, target) = it;

      if let Some(target_i) = target_names.into_iter().position(|name: &&str| *name == target.to_string().as_str()) {
        unsafe{*nids_arr.get_unchecked_mut(target_i) = nid_or_distribution}
      }
      else {panic!("target {} not found: ", target);}
    })
}