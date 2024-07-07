// use godot::{builtin::*, engine::{IResource, Resource, SpriteFrames}, obj::{Base, Gd}, register::{godot_api, Export, GodotClass, GodotConvert, Var}};
// #[derive(GodotClass)]
// #[class(tool, base=Resource)]
// pub struct SpriteData {
//   base: Base<Resource>,
//   #[var] id: StringName,
//   #[export] name: StringName, 
//   #[export] frames: Option<Gd<SpriteFrames>>, 
//   #[export] sex: super::Sex,
//   #[export] animation_states: PackedByteArray, //causa problemas, esperar que actualicen la api de gdext
//   #[export] offset_global: Vector2, 
//   #[export] offset_down: Vector2, 
//   #[export] offset_up: Vector2, 
//   #[export] offset_sideways: Vector2, 
//   #[export] simmetrical_sideways: bool, 
//   #[export] width_frontally_sideways_height: Vector3 , 
// }
// #[godot_api]
// impl IResource for SpriteData {
//   fn init(base: Base<Resource>) -> Self {
//     Self {base,id:"".into(), name: todo!(), frames: todo!(), sex: todo!(), animation_states: todo!(), offset_global: todo!(), offset_down: todo!(), offset_up: todo!(), offset_sideways: todo!(), simmetrical_sideways: todo!(), width_frontally_sideways_height: todo!() }
//   }
// }
// #[godot_api]
// impl SpriteData {
//   pub fn base(&self) -> &Base<Resource> { &self.base }
//   pub fn id(&self) -> &StringName { &self.id }
// }
// use std::hash::{Hash, Hasher};

// impl Hash for SpriteData {
//   fn hash<H: Hasher>(&self, state: &mut H) {
//     state.write_u32(self.id.hash());
//   }
// }

// #[derive(GodotConvert, Var, Export)]
// #[godot(via = i64)]
// pub enum AnimationState {//chequear que esté igual que como está definido en GdScript
//   Idle = 0,
//   Walk,
//   Jog,
//   Extra0,
//   Extra1,
//   Extra2,
//   Extra3
// }
