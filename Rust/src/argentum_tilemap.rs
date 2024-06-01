use godot::engine::{ITileMap, TileMap};
use godot::prelude::*;
use godot::builtin::Dictionary;
use crate::world_matrix::WorldMatrix;


struct InitializedState{

}


#[derive(GodotClass)]
#[class(base=TileMap)]
struct ArgentumTileMap{
  tiles_data: Dictionary,

  world_matrix: WorldMatrix,
  base: Base<TileMap>,

}
#[godot_api]
impl ITileMap for ArgentumTileMap{
  fn init(base: Base<TileMap>) -> Self{
    godot_print!("hello");
    Self{ base, }
  }
}



impl ArgentumTileMap{
    
}