use godot::prelude::*;

mod rust_tilemap;
mod formation_generator;
mod matrix;
pub mod safe_vec;
pub mod uns_vec;
pub mod world_matrix;
pub mod tiling;
pub mod fractured_continent_generator;

struct ArgentumExtension;

#[gdextension]
unsafe impl ExtensionLibrary for ArgentumExtension{}

extern crate strum; // 0.10.0
#[macro_use]
extern crate strum_macros; // 0.10.0

use godot::engine::Node;

#[derive(GodotClass)]
#[class(base=Node)]
struct Test {
    node: Base<Node>,
}

#[godot_api]
impl INode for Test{

    fn init(node: Base<Node>) -> Self {
        godot_print!("Rust module loaded");
        Self { node }
    }
    fn ready(&mut self) {}
}
