use godot::{register::GodotClass, prelude::*};
use rand_distr::{Distribution};
use rand_distr::weighted::WeightedAliasIndex;
use rand_pcg::Lcg128Xsl64; use std::fmt::{self};
use std::hash::{Hash, Hasher};
pub use crate::utils::uns_vec::UnsVec;

#[derive(Clone, PartialEq, Copy, Debug)]
pub struct TileUnid(pub u16);
impl TileUnid{pub const NULL: TileUnid = TileUnid(u16::MAX);}
impl Default for TileUnid {fn default() -> Self {TileUnid::NULL}}
impl Hash for TileUnid {fn hash<H: Hasher>(&self, state: &mut H) {state.write_u16(self.0);}}
impl fmt::Display for TileUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Tunid{}", self.0)}}//alt+z

use strum_macros::EnumIter;
#[derive(GodotConvert, Var, Export, Clone, Copy, EnumCount, Debug, Display, EnumIter, VariantNames)] #[godot(via = i32)]
pub enum TileZLevel {Soil = 0, Water=1, Floor=2, Stain=3, Structure=4, Roof=5,}
impl Default for TileZLevel {fn default() -> Self {Self::Soil}} impl Hash for TileZLevel {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i8(*self as i8)}}

impl TileZLevel{
  pub fn new(i: i32) -> Self {
    match i {
      0 => Self::Soil,
      1 => Self::Water,
      2 => Self::Floor,
      3 => Self::Stain,
      4 => Self::Structure,
      5 => Self::Roof,
      _ => panic!("TileZLevel::new: invalid i={}", i),
    }
  }
}


#[derive(GodotClass, Debug)]
#[class(tool, base=Resource)]
pub struct Tile {
  base: Base<Resource>,
  #[var] mid: StringName,
  #[export] mz_level: TileZLevel,
  #[export] msource_atlas: i32,
  #[export] morigin_position: Vector2i,
  #[export] mmodulo_tiling_area: Vector2i,
  #[export] malternative_id: i32,
  #[export] mrandom_scale_range: Vector4,// tal vez es mejor volver a los bushes y trees escenas para poder hacer esto
  #[export] mflipped_at_random: bool,

  pub unid: Option<TileUnid>,
}
#[godot_api] impl Tile {//TODO hacer
  pub fn base(&self) -> &Base<Resource> { &self.base }
  pub fn id(&self) -> &StringName { &self.mid }
  pub fn z_level(&self) -> TileZLevel { self.mz_level }
  pub fn source_atlas(&self) -> i32 { self.msource_atlas }
  pub fn origin_position(&self) -> Vector2i { self.morigin_position }
  pub fn modulo_tiling_area(&self) -> Vector2i { self.mmodulo_tiling_area }
  pub fn alternative_id(&self) -> i32 { self.malternative_id }
  pub fn random_scale_range(&self) -> Vector4 { self.mrandom_scale_range }
  pub fn flipped_at_random(&self) -> bool { self.mflipped_at_random }

  #[func]
  fn validate(&self) -> bool {
    let err_msg = format!("modulo tiling area for Tile id={} must be bigger or equal than (1,1)", self.id());
    let modulo_tiling_area: UnsVec = match self.mmodulo_tiling_area.try_into() {
        Ok(area) => area,
        Err(_) => {godot_error!("{}", err_msg); return false;},
    };
    if modulo_tiling_area.all_bigger_than_min(1).is_err() {
        godot_error!("{}", err_msg); return false;
    }
    true
  }
}
#[godot_api]
impl IResource for Tile{
  fn init(base: Base<Resource>) -> Self {
    Self {base, mid: StringName::from(""), mz_level: TileZLevel::Soil, msource_atlas: -1, morigin_position: Vector2i{x: 0, y: 0}, mmodulo_tiling_area: Vector2i{x: 1, y: 1}, 
    malternative_id: 0, mrandom_scale_range: Vector4{x: 1.0, y: 1.0, z: 1.0, w: 1.0}, mflipped_at_random: false, unid: None,}
  }
}
impl Into<TileDto> for Gd<Tile> {fn into(self) -> TileDto {let gd_tile = self.bind(); TileDto { id: gd_tile.id().clone(), z_level: gd_tile.z_level(), source_atlas: gd_tile.source_atlas(), origin_position: gd_tile.origin_position(), modulo_tiling_area: gd_tile.modulo_tiling_area().try_into().expect("error negative"), alternative_id: gd_tile.alternative_id(), random_scale_range: gd_tile.random_scale_range(), flipped_at_random: gd_tile.flipped_at_random() }}}
pub struct TileDto{
  pub id: StringName, pub z_level: TileZLevel, pub source_atlas: i32, pub origin_position: Vector2i,
  pub modulo_tiling_area: UnsVec, pub alternative_id: i32, pub random_scale_range: Vector4, pub flipped_at_random: bool,
}
#[derive(GodotClass)]#[class(init, tool, base=Resource)]
pub struct TileSelection {
  base: Base<Resource>,
  #[var] mid: StringName,
  #[export] mtargets: Array<StringName>,
  #[export] muse_distribution: Array<bool>,//false:then Tile, true: then TileDistribution
  #[export] mtiles: Array<Gd<Tile>>,
  #[export] mtiles_distributions: Array<Gd<TileDistribution>>,
}
#[godot_api] impl TileSelection {
  pub fn id(&self) -> &StringName { &self.mid }
  pub fn targets(&self) -> &Array<StringName> {&self.mtargets}
  
  pub fn use_distribution(&self) -> &Array<bool>{&self.muse_distribution}
  pub fn tiles(&self) -> &Array<Gd<Tile>> {&self.mtiles}
  pub fn tiles_distributions(&self) -> &Array<Gd<TileDistribution>> {&self.mtiles_distributions}

  #[func]
  pub fn validate(&self) -> bool {
    self.mtargets.len() >= self.tiles_distributions().len() &&
    self.mtargets.len() >= self.tiles().len() &&
    self.tiles_distributions().len() == self.use_distribution().len()
  }
}
pub struct GdTileSelectionIterator{tile_selection: Gd<TileSelection>, current_index: usize}
impl GdTileSelectionIterator{pub fn new(tile_selection: Gd<TileSelection>) -> Self {Self{tile_selection, current_index: 0}}}
impl Iterator for GdTileSelectionIterator { type Item = UnidOrDist;
  fn next(&mut self) -> Option<Self::Item> {
    let tile_selection = self.tile_selection.bind();
    if self.current_index >= tile_selection.targets().len() {return None;}
    unsafe{
      let result = if tile_selection.muse_distribution.get(self.current_index).is_some_and(|x| x) {
        tile_selection.mtiles_distributions.get(self.current_index)
          .expect(format!("No tile distribution available for target \"{}\" (i={}) in TileSelection with id={}", tile_selection.targets().get(self.current_index).unwrap_unchecked(), self.current_index, self.tile_selection.bind().id()).as_str())
          .try_into()
      } else {
        tile_selection.mtiles.get(self.current_index)
          .expect(format!("No tile available for target \"{}\"(i={}) in TileSelection with id={}", tile_selection.targets().get(self.current_index).unwrap_unchecked(), self.current_index, self.tile_selection.bind().id()).as_str())
          .try_into()
      };
      self.current_index += 1;
      Some(result.unwrap())
    }
  }
}
#[derive(GodotClass)] #[class(tool, init, base=Resource)]
pub struct TileDistribution {
  base: Base<Resource>,
  #[var] mid: StringName,
  #[export] mtiles: Array<Gd<Tile>>,
  #[export] mweights: PackedInt32Array,
}
#[godot_api] #[allow(dead_code)]
impl TileDistribution {
  pub fn base(&self) -> &Base<Resource> { &self.base }
  pub fn id(&self) -> &StringName { &self.mid }
  #[func]
  pub fn validate(&self) -> bool {//hacer que devuelva un stringname con el error?
    ! self.mtiles.is_empty()
    && self.mtiles.len() == self.mweights.len() 
    && self.mweights.as_slice().iter().all(|x| *x >= 0) 
  }
}
#[derive(Debug)] enum ErrTileOrDistribution{Tile(StringName),Distribution(StringName),}

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
impl TryFrom<Gd<TileDistribution>> for UnidOrDist {type Error = TileDistributionError;
  fn try_from(mut val: Gd<TileDistribution>) -> Result<UnidOrDist, Self::Error> {

    let gd_tile_dist = val.bind();

    if gd_tile_dist.mtiles.is_empty() {
      return Err(TileDistributionError::EmptyTilesArr{id: gd_tile_dist.id().clone()});
    }
    if gd_tile_dist.mweights.len() > gd_tile_dist.mtiles.len() {
      return Err(TileDistributionError::MoreWeightsThanTiles{id: gd_tile_dist.id().clone()});
    }
    if gd_tile_dist.mweights.is_empty() || gd_tile_dist.mweights.as_slice().iter().all(|&w| w <= 0) {
      godot_warn!("TileDistributon id={}: None of the weights has a strictly positive value, setting all values to 1", gd_tile_dist.id());
      let tiles_len: usize = gd_tile_dist.mtiles.len();
      let ori_weights_len = gd_tile_dist.mweights.len();
      std::mem::drop(gd_tile_dist);
      let mut gd_tile_dist = val.bind_mut();
      if ori_weights_len < tiles_len {
        let difference = tiles_len - ori_weights_len;
        for _ in 0..difference {
          gd_tile_dist.mweights.push(1);
        }
      }
      for i in 0..ori_weights_len{
        unsafe{*(gd_tile_dist.mweights.as_mut_slice().get_unchecked_mut(i)) = 1}   
      }
    }
    let gd_tile_dist = val.bind();
    if gd_tile_dist.mtiles.iter_shared().any(|tile| tile.bind().unid.is_none()){
      return Err(TileDistributionError::MissingUnid{unid_or_dist: ErrTileOrDistribution::Distribution(gd_tile_dist.id().clone())});
    }
    if gd_tile_dist.mtiles.len() == 1 {unsafe{
      let tile = gd_tile_dist.mtiles.get(0).unwrap_unchecked();
      return Ok(UnidOrDist::Nid((tile.bind().unid.unwrap_unchecked(), tile.bind().mz_level)));
    }}
    #[allow(unused_unsafe)]
    unsafe{
      let mut choices: Vec<(TileUnid, TileZLevel)> = Vec::new();
      let sampler: WeightedAliasIndex<i32> = WeightedAliasIndex::new(gd_tile_dist.mweights.as_slice().to_vec()).unwrap();
      choices.reserve_exact(gd_tile_dist.mtiles.len());

      for tile in gd_tile_dist.mtiles.iter_shared(){
        let tile = tile.bind();
        choices.push((tile.unid.expect(&format!("Tile id={}: Unid not assigned", tile.id())), tile.z_level()));
      }

      Ok(UnidOrDist::Dist(DiscreteDistribution::new(choices, sampler)))
    }
  }
}
impl TryFrom<Gd<Tile>> for UnidOrDist {type Error = TileDistributionError;
  fn try_from(value: Gd<Tile>) -> Result<UnidOrDist, TileDistributionError> {

    let tile = value.bind();
    let (unid, z_level) = (tile.unid, tile.mz_level);
    match (unid, z_level){
      (Some(unid),z_level) => Ok(UnidOrDist::Nid((unid, z_level))),
      (None, _) => Err(TileDistributionError::MissingUnid{unid_or_dist: ErrTileOrDistribution::Tile(tile.mid.clone())}),
    }
  }
}
pub struct DiscreteDistribution{choices: Vec<(TileUnid, TileZLevel)>, sampler: WeightedAliasIndex<i32>,}
impl DiscreteDistribution{
  pub fn new(choices: Vec<(TileUnid, TileZLevel)>, sampler: WeightedAliasIndex<i32>,) -> Self {Self {choices, sampler}}
  pub fn sample(&self, rng: &mut Lcg128Xsl64) -> (TileUnid, TileZLevel){unsafe{self.choices.get_unchecked(self.sampler.sample(rng)).clone()}}
}
pub enum UnidOrDist{Nid((TileUnid, TileZLevel)), Dist(DiscreteDistribution)}
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
  assert!(target_names.len() <= tile_selection.targets().len(), "TileSelection(id={} len={}) doesn't provide for all target_names(len={})", tile_selection.mid, tile_selection.targets().len(), target_names.len());

  GdTileSelectionIterator::new(arg_tile_selection.clone()).zip(tile_selection.targets().iter_shared())
    .for_each(|it: (UnidOrDist, StringName)| {
      let (nid_or_distribution, target) = it;

      if let Some(target_i) = target_names.into_iter().position(|name: &&str| *name == target.to_string().as_str()) {
        unsafe{*nids_arr.get_unchecked_mut(target_i) = nid_or_distribution}
      }
      else {panic!("target {} not found: ", target);}
    })
}

