use std::{collections::HashSet, hash::*};

use godot::{builtin::{Array, GString, StringName, Vector2i}, classes::{GDScript, IResource, Resource, Texture2D}, obj::{Base, Gd}, register::{godot_api, GodotClass}};

use crate::formation_generation::{Tile, TileDto};

use super::diet::Diet;

#[derive(GodotClass)]
#[class(tool, base=Resource)] 
pub struct BasicRace {
  base: Base<Resource>,
  #[var] mid: StringName,
  #[export] micon: Option<Gd<Texture2D>>,
  #[export] mname: GString,
  #[export] mdescription: GString,
  #[export] mdemonym: GString,
  #[export] msingular_denomination: GString,
  #[export] mplural_denomination: GString,
  #[export] mdefault_being_names: Array<GString>,
  #[export] mmales_ratio: f32,
  #[export] mcan_equip_tools: bool,
  #[export] mfilth_generation: f32,
  #[export] mmax_encumberance_multiplier: f32,
  #[export] mcomfortable_temp_range: Vector2i,
  #[export] mai_process: Option<Gd<GDScript>>,//fallback si beingkind no lo tiene
  #[export] mhunger_size: f32,
  #[export] mhunger_rate: f32,
  #[export] mdiets: Array<Gd<Diet>>,
  #[export] mcan_walk_on: Array<Gd<Tile>>,
  #[export] mcan_vent: bool,
  #[export] mcan_climb: bool,

  //TODO hacer sets de tiles whitelisted comúnes para reutilizar (hacerlo un array const definido en godot usando preload?)
  #[export] mwhitelisted_tiles_for_spawning: Array<Gd<Tile>>, //TODO SETTER QUE ACTUALIZE whitelisted_tiles_for_spawning TMB
  #[export] mblacklisted_tiles_for_spawning: Array<Gd<Tile>>,//toma precedencia si la tile aparece en whitelisted

  rust_whitelisted_tiles_for_spawning: HashSet<TileDto>, rust_blacklisted_tiles_for_spawning: HashSet<TileDto>,
}
#[godot_api]
impl IResource for BasicRace {
  fn init(base: Base<Resource>) -> Self {
    Self {base,
      mid: "".into(),
      micon: None,
      mname: "".into(), mdescription: "".into(), mdemonym: "".into(), msingular_denomination: "".into(), mplural_denomination: "".into(), mdefault_being_names: Array::new(),
      mmales_ratio: 0.5,
      mcan_climb: true,
      mcan_equip_tools: true,
      mcan_vent: true,
      mcan_walk_on: Array::new(),
      mmax_encumberance_multiplier: 1.0,
      mhunger_rate: 1.0, mhunger_size: 5.0,
      mdiets: Array::new(),
      mfilth_generation: 0.0,
      mcomfortable_temp_range: Vector2i { x: 16, y: 23 },
      mai_process: None, //Some(godot::tools::load::<GDScript>("res://scripts/beings/default_ai_process.gd")),
      mwhitelisted_tiles_for_spawning: Array::new(), mblacklisted_tiles_for_spawning: Array::new(),
      rust_whitelisted_tiles_for_spawning: HashSet::new(), rust_blacklisted_tiles_for_spawning: HashSet::new(),
    }
  }
}
#[godot_api]
impl BasicRace {
  pub fn base(&self) -> &Base<Resource> {&self.base}
  pub fn id(&self) -> &StringName {&self.mid}


}

impl Hash for BasicRace {
  fn hash<H: Hasher>(&self, state: &mut H) {
    state.write_u32(self.mid.hash());
  }
}

