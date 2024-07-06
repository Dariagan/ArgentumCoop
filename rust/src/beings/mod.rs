use std::{fmt, hash::*};

use godot::builtin::StringName;

pub mod being_builder;
mod spawn_weights_matrix;


#[derive(PartialEq, Eq, Clone)]
pub struct BeingKindStrId(pub StringName);
impl Hash for BeingKindStrId {fn hash<H: Hasher>(&self, state: &mut H) {state.write_u32(self.0.hash())}}
impl fmt::Display for BeingKindStrId {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bstrid{}", self.0)}}


#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct BeingUnid(pub i64);
impl Hash for BeingUnid {fn hash<H: Hasher>(&self, state: &mut H) {state.write_i64(self.0);}}
impl fmt::Display for BeingUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bunid{}", self.0)}}

#[derive(PartialEq, PartialOrd, Eq, Ord, Clone, Copy)]
pub struct BeingKindUnid(pub u16);
impl Hash for BeingKindUnid {fn hash<H: Hasher>(&self, state: &mut H) {state.write_u16(self.0);}}
impl fmt::Display for BeingKindUnid {fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {write!(f, "Bkindunid{}", self.0)}}