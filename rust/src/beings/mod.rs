use std::{fmt, hash::*};

pub mod being_builder;
mod spawn_weights_matrix;





#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct BeingUnid(pub i64);
impl Hash for BeingUnid {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i64(self.0);}}
impl fmt::Display for BeingUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bunid{}", self.0)}}