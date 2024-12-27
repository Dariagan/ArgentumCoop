use godot::builtin::StringName;
use godot::builtin::{Array, Dictionary, GString, Vector2i};
use godot::classes::{GDScript, Node, Object, ResourcePreloader, Texture2D};
use godot::obj::{Gd, NewGd};
use godot::register::property::Var;
use godot::register::{Export, GodotConvert, Var};
use godot::tools::load;
use std::collections::HashSet;
use std::str::FromStr;
use std::{fmt, hash::*};

pub mod being_builder; pub mod diet; pub mod spawn_weights_matrix; pub mod basic_race; pub mod sprite_data;

#[derive(PartialEq, Eq, Clone, Hash)]
pub struct BeingGenTemplIdAndFac{pub being_gen_templ_id: StringName, pub fac_id: StringName} 
impl fmt::Display for BeingGenTemplIdAndFac {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bstrid{} Facstrid{}", self.being_gen_templ_id, self.fac_id)}}

#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct BeingUnid(pub i64); impl Hash for BeingUnid {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i64(self.0);}}
impl fmt::Display for BeingUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bunid{}", self.0)}}

#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct BeingGenTemplateUnid(pub u16);impl Hash for BeingGenTemplateUnid {fn hash<H: Hasher>(&self, state: &mut H){state.write_u16(self.0);}}
impl fmt::Display for BeingGenTemplateUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bkindunid{}", self.0)}}


pub fn cache_being_gen_templates_soil_tiles_unids(tilemap: &RustTileMap) {
  let obj = godot::classes::Engine::singleton().get_singleton("Global").expect("couldn't retrieve /Global singleton");

  let being_gen_templates: Dictionary = obj.get("being_gen_templates").try_to().expect("couldn't get dict being_gen_templates from Global");

  for being_gen_templ in being_gen_templates.values_array().iter_shared(){
    let mut being_gen_templ: Gd<RustBeingGenTemplate> = being_gen_templ.try_to().expect("couldn't do Gd<RustBeingGenTemplate> = being_gen_templ.try_to()");
    being_gen_templ.bind_mut().cache_allowed_soil_tiles_unids(tilemap);
  }
}


#[allow(dead_code)]
pub fn retrieve_being_gen_template_from_id(being_gen_template_id: &StringName) -> Gd<RustBeingGenTemplate> {
  let obj = godot::classes::Engine::singleton().get_singleton("Global").expect("couldn't retrieve /Global singleton");

  let being_gen_templates: Dictionary = obj.get("being_gen_templates").try_to().expect("couldn't get dict being_gen_templates from Global");

  being_gen_templates.get(being_gen_template_id.clone()).expect(format!("couldn't find {} in Global's being_gen_templates dict", being_gen_template_id).as_str())
    .try_to().expect(format!("couldn't parse object at being_gen_templates[\"{}\"] as a RustBeingGenTemplate", being_gen_template_id).as_str())
}

use godot::{classes::{IResource, Resource}, obj::Base,register::godot_api, register::GodotClass,};
#[derive(GodotClass)] #[class(base=Resource)]
pub struct RustBeingGenTemplate {
  base: Base<Resource>,
  #[var] mid: StringName,
  //if none specified (array is empty), race defaults are used
  //TODO hacer sets de tiles whitelisted comúnes para reutilizar (hacerlo un array const definido en godot usando preload?)
  #[export] mwhitelisted_tiles_for_spawning: Array<StringName>,
  //TODO SETTER QUE ACTUALIZE whitelisted_tiles_for_spawning TMB

  allowed_soil_tiles_unids: Option<Vec<TileUnid>>,

}
#[godot_api]
impl IResource for RustBeingGenTemplate {
  fn init(base: Base<Resource>) -> Self {
    Self {
      base: base, mid: StringName::from(""),
      mwhitelisted_tiles_for_spawning: Array::new(),
      allowed_soil_tiles_unids: None
    }
  }
}
#[godot_api]
impl RustBeingGenTemplate {
  pub fn base(&self) -> &Base<Resource> {&self.base}
  pub fn id(&self) -> &StringName {&self.mid}

  pub fn cache_allowed_soil_tiles_unids(&mut self, tilemap: &RustTileMap){
    self.allowed_soil_tiles_unids = Some(Vec::new());
    for soil_tile_id in self.mwhitelisted_tiles_for_spawning.iter_shared() {
      for (i, tile_dto) in tilemap.tile_nid_mapping().iter().enumerate(){
        if tile_dto.id == soil_tile_id{
          unsafe{
            self.allowed_soil_tiles_unids.as_mut().unwrap_unchecked().push(TileUnid(i as u16));
          }
        }
      }
    }
  }
  pub fn allowed_soil_tiles_unids(&self) -> Option<&Vec<TileUnid>>{
    self.allowed_soil_tiles_unids.as_ref()
  }
}

use crate::formation_generation::{Tile, TileDto};
use crate::rust_tilemap::RustTileMap;
use crate::tiling::TileUnid;
impl Hash for RustBeingGenTemplate {fn hash<H: Hasher>(&self, state: &mut H) {state.write_u32(self.mid.hash());}}

#[derive(GodotConvert, Var, Export)] #[godot(via = i8)] pub enum Sex {Male, Female, Any,}

//TODO hacer que se puedan instanciar factions NPC nuevas desde el rust-side