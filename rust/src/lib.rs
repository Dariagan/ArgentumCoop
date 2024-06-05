use godot::prelude::*;

mod rust_tilemap;
mod formation_generator;
mod matrix;
pub mod safe_vec;
pub mod uns_vec;
pub mod world_matrix;
pub mod tile;
pub mod fractured_formation_generator;

struct ArgentumExtension;

#[gdextension]
unsafe impl ExtensionLibrary for ArgentumExtension{}

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
