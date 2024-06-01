use crate::{safevec::SafeVec, world_matrix::WorldMatrix};
use godot::builtin::Dictionary;
use godot::engine::{resource, RefCounted, Resource};
use godot::obj::{Gd, GdRef};

pub trait IFormationGenerator {
  fn generate(
      &mut self,
      origin: SafeVec,
      size: SafeVec,
      tile_selection_set: &GdRef<Resource>,
      seed: u64,
      data: &mut Dictionary,
  ) -> WorldMatrix;
}

pub fn get_border_closeness_factor(coords: &SafeVec, world_size: &SafeVec, power: Option<f64>) -> f64 {
  let power = power.unwrap_or(3.0);

  let horizontal_border_closeness: f64 = ((coords.lef as f64 - world_size.lef as f64 / 2.0) / (world_size.lef as f64 / 2.0)).abs().powf(power);
  let vertical_border_closeness: f64 = ((coords.right as f64 - world_size.right as f64 / 2.0) / (world_size.right as f64 / 2.0)).abs().powf(power);

  return horizontal_border_closeness.max(vertical_border_closeness);
}
