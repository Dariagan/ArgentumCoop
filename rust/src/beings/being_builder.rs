use godot::builtin::{Dictionary, StringName, Vector3};

pub struct BeingBuilder{
  name: StringName,
  faction_id: Option<StringName>,
  race_id: StringName,
  klass_id: StringName,
  body_id: StringName,
  head_id: StringName,
  head_scale: Vector3,
  body_scale: Vector3,
  health_multiplier: f64,
}
#[allow(dead_code)]
impl BeingBuilder {

  pub fn new() -> Self { 
    Self{name: "random".into(), faction_id: None, race_id: "random".into(), klass_id: "random".into(), 
      body_id: "random".into(), head_id: "random".into(), head_scale: Vector3::ONE, body_scale: Vector3::ONE, health_multiplier: 1.0 }
  }
  pub fn name(&mut self, being_name: StringName) -> &mut Self {self.name = being_name; self}
  pub fn faction_id(&mut self, faction_id: StringName) -> &mut Self {self.faction_id = Some(faction_id); self}
  pub fn race_id(&mut self, race_id: StringName) -> &mut Self {self.race_id = race_id; self}
  pub fn klass_id(&mut self, klass_id: StringName) -> &mut Self {self.klass_id = klass_id; self}
  pub fn head_id(&mut self, head_id: StringName) -> &mut Self {self.head_id = head_id; self}
  pub fn body_id(&mut self, body_id: StringName) -> &mut Self {self.body_id = body_id; self}
  pub fn head_scale(&mut self, head_scale: Vector3) -> &mut Self {self.head_scale = head_scale; self}
  pub fn body_scale(&mut self, body_scale: Vector3) -> &mut Self {self.body_scale = body_scale; self}
  pub fn health_multiplier(&mut self, health_multiplier: f64) -> &mut Self {self.health_multiplier = health_multiplier; self}

  pub fn build(&self) -> Dictionary{
    let mut dict = Dictionary::new();
    dict.set("name", self.name.clone());
    dict.set("klass_id", self.klass_id.clone());
    dict.set("race_id", self.race_id.clone());
    dict.set("head_id", self.head_id.clone());
    dict.set("body_id", self.body_id.clone());
    dict.set("faction_id", <Option<godot::prelude::StringName> as Clone>::clone(&self.faction_id).expect("faction id not set").clone());
    dict.set("health_multiplier", self.health_multiplier); 

    if ! self.health_multiplier.is_sign_positive() {panic!("health multiplier less or equal to zero for dict:\n{}", dict)}
    dict
  }

}

