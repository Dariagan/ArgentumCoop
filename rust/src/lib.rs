use godot::prelude::*;

mod rust_tilemap;
mod formation_generator;
mod matrix;
mod safe_vec;
mod uns_vec;
mod world_matrix;
mod tiling;
mod fractured_continent_generator;
mod utils;

struct ArgentumExtension;

#[gdextension]
unsafe impl ExtensionLibrary for ArgentumExtension{}

extern crate strum; // 0.10.0
#[macro_use]
extern crate strum_macros; // 0.10.0

