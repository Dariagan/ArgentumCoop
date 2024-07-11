use std::cmp::Ordering;
use std::fmt;
use std::hash::{Hash, Hasher};
use std::ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Neg, Sub, SubAssign};

use godot::builtin::{Vector2, Vector2i};

use crate::utils::safe_vec::SafeVec;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct UnsVec {pub lef: u32, pub right: u32}

#[allow(dead_code)]
impl UnsVec {
  pub const ZERO: UnsVec = UnsVec{lef:0,right:0};
  pub fn new(lef: u32, right: u32) -> Self {UnsVec { lef, right }}

  pub fn add_assign_lef(&mut self, other: &Self) {self.lef += other.lef;}
  pub fn add_assign_right(&mut self, other: &Self) {self.right += other.right;}
  pub fn sum_lef(&self, i: u32) -> Self {UnsVec {lef: self.lef + i, right: self.right}}
  pub fn sum_right(&self, j: u32) -> Self {UnsVec {lef: self.lef, right: self.right + j,}}
  pub fn compare_lef(&self, other: &Self) -> Ordering {self.lef.cmp(&other.lef)}
  pub fn compare_right(&self, other: &Self) -> Ordering {self.right.cmp(&other.right)}
  pub fn is_strictly_smaller_than(&self, other: Self) -> bool {self.lef < other.lef && self.right < other.right}
  pub fn is_strictly_bigger_than(&self, other: &Self) -> bool {self.lef > other.lef && self.right > other.right}
  pub fn is_bigger_or_equal_than(&self, other: Self) -> bool {self.lef >= other.lef && self.right >= other.right}
  pub fn flat_index(&self, size: &UnsVec) -> usize{(self.lef * size.lef + self.right) as usize}
  pub fn swap_components(&mut self) -> Self {
    std::mem::swap(&mut self.lef, &mut self.right);
    *self
  }
  
  pub fn length(&self) -> f64 {self.distance_to(&UnsVec { lef: 0, right: 0 })}
  pub fn length_f32(&self) -> f32 {self.distance_to(&UnsVec { lef: 0, right: 0 }) as f32}
  
  pub fn area(&self) -> usize {(self.lef * self.right) as usize}
  pub fn all_bigger_than_min(&self, min: u32) -> Result<Self, &str> {
  if self.lef >= min && self.right >= min {Ok(*self)} else {Err("all bigger than min")}
  }
  pub fn mod_unsv(&self, divider: UnsVec) -> UnsVec {UnsVec{lef: self.lef%divider.lef, right: self.right%divider.right}}
  pub fn mod_u32(&self, divider: u32) -> UnsVec {UnsVec{lef: self.lef%divider, right: self.right%divider}}

  pub fn within_bounds_centered(&self, checked: SafeVec) -> bool{
    let top_left: SafeVec = SafeVec::from(*self)/2.neg();
    let bottom_right: SafeVec = SafeVec::from(*self)/2;
    let checked: SafeVec = checked.into();
    checked.is_equal_or_bigger_than(top_left) && checked.is_strictly_smaller_than(bottom_right)
  }

  pub fn distance_to(&self, other: &Self) -> f64 {
  (((self.lef - other.lef).pow(2) + (self.right - other.right).pow(2)) as f64).sqrt()
  }
  pub fn distance_squared_to(&self, other: &Self) -> usize {
  ((self.lef - other.lef).pow(2) + (self.right - other.right).pow(2)) as usize
  }
}

impl Default for UnsVec {
  fn default() -> Self {
    UnsVec{lef:0, right:0}
  }
}
impl Add for UnsVec {
  type Output = UnsVec;
  fn add(self, other: UnsVec) -> UnsVec {
  UnsVec {
    lef: self.lef + other.lef,
    right: self.right + other.right,
  }
  }
}
impl AddAssign for UnsVec {
  fn add_assign(&mut self, other: UnsVec) {
  self.lef += other.lef;
  self.right += other.right;
  }
}
impl Sub for UnsVec {
  type Output = UnsVec;
  fn sub(self, other: UnsVec) -> UnsVec {
  UnsVec {
    lef: self.lef - other.lef,
    right: self.right - other.right,
  }
  }
}
impl SubAssign for UnsVec {
  fn sub_assign(&mut self, other: UnsVec) {
  self.lef -= other.lef;
  self.right -= other.right;
  }
}
impl Mul for UnsVec {
  type Output = UnsVec;
  fn mul(self, other: UnsVec) -> UnsVec {
  UnsVec {
    lef: self.lef * other.lef,
    right: self.right * other.right,
  }
  }
}
impl MulAssign for UnsVec {
  fn mul_assign(&mut self, other: UnsVec) {
  self.lef *= other.lef;
  self.right *= other.right;
  }
}
impl Div for UnsVec {
  type Output = UnsVec;
  fn div(self, other: UnsVec) -> UnsVec {
  UnsVec {
    lef: self.lef / other.lef,
    right: self.right / other.right,
  }
  }
}
impl DivAssign for UnsVec {
  fn div_assign(&mut self, other: UnsVec) {
  self.lef /= other.lef;
  self.right /= other.right;
  }
}
macro_rules! impl_operations_for_UnsVec {
  ($($t:ty),*) => {
  $(
    impl Mul<$t> for UnsVec {
    type Output = UnsVec;
    fn mul(self, number: $t) -> UnsVec {
      UnsVec {
        lef: (self.lef as f64 * number as f64) as u32,
        right: (self.right as f64 * number as f64) as u32,
      }
    }
    }
    impl MulAssign<$t> for UnsVec {
    fn mul_assign(&mut self, number: $t) {
      self.lef = (self.lef as f64 * number as f64) as u32;
      self.right = (self.right as f64 * number as f64) as u32;
    }
    }
    impl Div<$t> for UnsVec {
    type Output = UnsVec;
    fn div(self, number: $t) -> UnsVec {
      UnsVec {
        lef: self.lef / number as u32,
        right: self.right / number as u32,
      }
    }
    }
    impl DivAssign<$t> for UnsVec {
      fn div_assign(&mut self, number: $t) {
        self.lef /= number as u32;
        self.right /= number as u32;
      }
    }
  )*
  }
}
impl_operations_for_UnsVec!(i8, i16, i32, i64, i128, isize, u8, u16, u32, u64, u128, usize);
impl PartialOrd for UnsVec {
  fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
    Some(self.cmp(other))
  }
}
impl Ord for UnsVec {
  fn cmp(&self, other: &Self) -> Ordering {
    match self.lef.cmp(&other.lef) {
    Ordering::Equal => self.right.cmp(&other.right),
    other => other,
    }
  }
}
impl Hash for UnsVec {
  fn hash<H: Hasher>(&self, state: &mut H) {
    state.write_u32(self.lef*31 + self.right)
  }
}
impl fmt::Display for UnsVec {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "UV({}, {})", self.lef, self.right)
  }
}
impl From<(u32, u32)> for UnsVec {
  fn from(value: (u32, u32)) -> Self {
    Self { lef: value.0, right: value.1 }
  }
}
impl From<SafeVec> for UnsVec {
  fn from(safe_vec: SafeVec) -> UnsVec{
    UnsVec { lef: safe_vec.lef as u32, right: safe_vec.right as u32 }
  }
}
impl TryFrom<Vector2i> for UnsVec {
  type Error = &'static str;
  fn try_from(godot_vector: Vector2i) -> Result<Self, Self::Error>{
    if godot_vector.x.is_negative() || godot_vector.y.is_negative() {
      Err("passed Vector2i must be non-negative")
    }
    else{
      Ok(UnsVec { lef: godot_vector.x as u32, right: godot_vector.y as u32})
    }
  }
}
impl Into<Vector2i> for UnsVec {
  fn into(self) -> Vector2i {
    Vector2i { x: self.lef as i32, y: self.right as i32}
  }
}
impl Into<Vector2> for UnsVec {
  fn into(self) -> Vector2 {
    Vector2 { x: self.lef as f32, y: self.right as f32}
  }
}
impl Into<String> for UnsVec {
  fn into(self) -> String {
    format!("({}, {})", self.lef, self.right)
  }
}

