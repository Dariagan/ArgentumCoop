use diet::Diet;
use godot::builtin::StringName;
use godot::builtin::{Array, Dictionary, GString, Vector2i};
use godot::engine::{GDScript, ResourcePreloader, Texture2D};
use godot::obj::{Gd, NewGd};
use godot::register::{Export, GodotConvert, Var};
use godot::tools::load;
use std::collections::HashSet;
use std::str::FromStr;
use std::{fmt, hash::*};

pub mod being_builder;
pub mod diet;
pub mod spawn_weights_matrix;
pub mod basic_race;
pub mod sprite_data;

#[derive(PartialEq, Eq, Clone)]
pub struct BeingKindStrId(pub StringName);
impl Hash for BeingKindStrId {
  fn hash<H: Hasher>(&self, state: &mut H) {
    state.write_u32(self.0.hash())
  }
}
impl fmt::Display for BeingKindStrId {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "Bstrid{}", self.0)
  }
}

#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct BeingUnid(pub i64);
impl Hash for BeingUnid {
  fn hash<H: Hasher>(&self, state: &mut H) {
    state.write_i64(self.0);
  }
}
impl fmt::Display for BeingUnid {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "Bunid{}", self.0)
  }
}

#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct BeingKindUnid(pub u16);
impl Hash for BeingKindUnid {
  fn hash<H: Hasher>(&self, state: &mut H) {
    state.write_u16(self.0);
  }
}
impl fmt::Display for BeingKindUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bkindunid{}", self.0)}}

use godot::{engine::{IResource, Resource}, obj::Base,register::godot_api, register::GodotClass,};
#[derive(GodotClass)]
#[class(tool, base=Resource)] // SAQUÉ TOOL
pub struct RustBeingKind {
  base: Base<Resource>,
  #[var]
  id: StringName,
  //if none specified (array is empty), race defaults are used
  //TODO hacer sets de tiles whitelisted comúnes para reutilizar (hacerlo un array const definido en godot usando preload?)
  #[export]
  whitelisted_tiles_for_spawning: Array<Gd<Tile>>, //TODO SETTER QUE ACTUALIZE whitelisted_tiles_for_spawning TMB
  #[export]
  blacklisted_tiles_for_spawning: Array<Gd<Tile>>,

  rust_whitelisted_tiles_for_spawning: HashSet<TileDto>,
  rust_blacklisted_tiles_for_spawning: HashSet<TileDto>,
}
#[godot_api]
impl IResource for RustBeingKind {
  fn init(base: Base<Resource>) -> Self {
    Self {
      base: base,
      id: StringName::from(""),
      whitelisted_tiles_for_spawning: Array::new(),
      blacklisted_tiles_for_spawning: Array::new(),
      rust_whitelisted_tiles_for_spawning: HashSet::new(),
      rust_blacklisted_tiles_for_spawning: HashSet::new(),
    }
  }
}
#[godot_api]
impl RustBeingKind {
  pub fn base(&self) -> &Base<Resource> {
    &self.base
  }
  pub fn id(&self) -> &StringName {
    &self.id
  }
}

use crate::formation_generation::{Tile, TileDto};
impl Hash for RustBeingKind {
  fn hash<H: Hasher>(&self, state: &mut H) {
    state.write_u32(self.id.hash());
  }
}



#[derive(GodotConvert, Var, Export)]
#[godot(via = i8)]
pub enum Sex {
    Male, 
    Female,
    Any,
}
