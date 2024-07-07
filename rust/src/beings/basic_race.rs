use std::{collections::HashSet, hash::*};

use godot::{builtin::{Array, GString, StringName, Vector2i}, engine::{GDScript, IResource, Resource, Texture2D}, obj::{Base, Gd}, register::{godot_api, GodotClass}};

use crate::formation_generation::{Tile, TileDto};

use super::diet::Diet;

#[derive(GodotClass)]
#[class(tool, base=Resource)] // SAQUÉ TOOL
pub struct BasicRace {
  base: Base<Resource>,
  #[var] id: StringName,
  #[export] icon: Option<Gd<Texture2D>>,
  #[export] name: GString,
  #[export] description: GString,
  #[export] demonym: GString,
  #[export] singular_denomination: GString,
  #[export] plural_denomination: GString,
  #[export] default_being_names: Array<GString>,
  #[export] males_ratio: f32,
  #[export] can_equip_tools: bool,
  #[export] filth_generation: f32,
  #[export] max_encumberance_multiplier: f32,
  #[export] comfortable_temp_range: Vector2i,
  #[export] ai_process: Option<Gd<GDScript>>,//fallback si beingkind no lo tiene
  #[export] hunger_size: f32,
  #[export] hunger_rate: f32,
  #[export] diets: Array<Gd<Diet>>,
  #[export] can_walk_on: Array<Gd<Tile>>,
  #[export] can_vent: bool,
  #[export] can_climb: bool,

  //TODO hacer sets de tiles whitelisted comúnes para reutilizar (hacerlo un array const definido en godot usando preload?)
  #[export] whitelisted_tiles_for_spawning: Array<Gd<Tile>>, //TODO SETTER QUE ACTUALIZE whitelisted_tiles_for_spawning TMB
  #[export] blacklisted_tiles_for_spawning: Array<Gd<Tile>>,//toma precedencia si la tile aparece en whitelisted

  rust_whitelisted_tiles_for_spawning: HashSet<TileDto>,
  rust_blacklisted_tiles_for_spawning: HashSet<TileDto>,
}
#[godot_api]
impl IResource for BasicRace {
  fn init(base: Base<Resource>) -> Self {
    Self {base,
      id: "".into(),
      icon: None,
      name: "".into(), description: "".into(), demonym: "".into(), singular_denomination: "".into(), plural_denomination: "".into(), default_being_names: Array::new(),
      males_ratio: 0.5,
      can_climb: true,
      can_equip_tools: true,
      can_vent: true,
      can_walk_on: Array::new(),
      max_encumberance_multiplier: 1.0,
      hunger_rate: 0.1, hunger_size: 5.0,
      diets: Array::new(),
      filth_generation: 0.0,
      comfortable_temp_range: Vector2i { x: 16, y: 23 },
      ai_process: None,
      whitelisted_tiles_for_spawning: Array::new(), blacklisted_tiles_for_spawning: Array::new(),
      rust_whitelisted_tiles_for_spawning: HashSet::new(), rust_blacklisted_tiles_for_spawning: HashSet::new(),
    }
  }
}
#[godot_api]
impl BasicRace {
  pub fn base(&self) -> &Base<Resource> {&self.base}
  pub fn id(&self) -> &StringName {&self.id}
}

impl Hash for BasicRace {
  fn hash<H: Hasher>(&self, state: &mut H) {
    state.write_u32(self.id.hash());
  }
}

