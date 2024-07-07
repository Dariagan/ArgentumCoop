use godot::{builtin::Dictionary, engine::{IResource, Resource}, obj::Base, register::{godot_api, GodotClass}};
#[derive(GodotClass)]
#[class(init, tool, base=Resource)]
pub struct Diet {
  base: Base<Resource>,
  #[export] nutrition_values: Dictionary,
  #[export] harm_values: Dictionary,
  #[export] parasitation_probs: Dictionary,
}

#[godot_api]
impl Diet {
  pub fn base(&self) -> &Base<Resource> { &self.base }
}
