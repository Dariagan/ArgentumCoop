mod tiling;
use godot::prelude::*;

mod utils;
mod rust_tilemap;
mod formation_generation;
mod beings;

struct ArgentumExtension;
#[gdextension]
unsafe impl ExtensionLibrary for ArgentumExtension{

  fn on_level_init(_level: InitLevel) {
    if _level == InitLevel::Scene{
      let formatted_time = chrono::Local::now().format("%H:%M:%S").to_string();
      godot_print!("{}: Rust module loaded", formatted_time);
    }
  }
}

extern crate strum; // 0.10.0
#[macro_use]
extern crate strum_macros; // 0.10.0