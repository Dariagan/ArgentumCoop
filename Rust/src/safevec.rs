use std::cmp::Ordering;
use std::{fmt, usize};
use std::hash::{Hash, Hasher};
use std::ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Neg, Sub, SubAssign};
use num::{integer, Num, PrimInt};

pub trait AllowedTypes: AddAssign + PrimInt{}

impl AllowedTypes for i32 {}
impl AllowedTypes for usize {}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct SafeVec<T: AllowedTypes = i32> {
  pub lef: T,
  pub right: T,
}

impl<T: AllowedTypes> SafeVec<T> {
  pub fn new(lef: T, right: T) -> Self {
    SafeVec { lef, right }
  }

  pub fn add_assign_lef(&mut self, other: &Self) {self.lef += other.lef;}

  pub fn add_assign_right(&mut self, other: &Self) {self.right += other.right;}

  pub fn sum_lef(&self, i: T) -> Self {
    SafeVec {
      lef: self.lef + i,
      right: self.right,
    }
  }

  pub fn sum_right(&self, j: T) -> Self {
    SafeVec {
      lef: self.lef,
      right: self.right + j,
    }
  }

  pub fn compare_lef(&self, other: &Self) -> Ordering {self.lef.cmp(&other.lef)}
  pub fn compare_right(&self, other: &Self) -> Ordering {self.right.cmp(&other.right)}
  pub fn is_strictly_smaller_than(&self, other: &Self) -> bool {self.lef < other.lef && self.right < other.right}
  pub fn is_strictly_bigger_than(&self, other: &Self) -> bool {self.lef > other.lef && self.right > other.right}
  pub fn is_any_comp_negative(&self) -> bool {self.lef < T::zero() || self.right < T::zero()}
  pub fn is_non_negative(&self) -> bool {self.lef >= T::zero() && self.right >= T::zero()}
  pub fn is_strictly_positive(&self) -> bool {self.lef > T::zero() && self.right > T::zero()}
  pub fn to_str(&self) -> String {format!("({}, {})", self.lef.to_i64().unwrap(), self.right.to_i64().unwrap())}

  pub unsafe fn flat_index(&self, size: &Self) -> usize{
    (self.lef*size.lef + self.right).to_usize().unwrap_unchecked()
  }

  pub unsafe fn length(&self) -> f64 {
    (self.lef * self.right).to_f64().unwrap_unchecked().sqrt()
  }

  pub fn area_usize(&self) -> usize {
    (self.lef * self.right).to_usize().unwrap()
  }

  pub unsafe fn area_f64(&self) -> f64 {
    (self.lef * self.right).to_f64().unwrap_unchecked()
  }


  pub unsafe fn distance_to(&self, other: &Self) -> f64 {
    (((self.lef - other.lef).pow(2) + (self.right - other.right).pow(2)).to_f64()).unwrap_unchecked().sqrt()
  }

  pub unsafe fn distance_squared_to(&self, other: &Self) -> u128 {
    ((self.lef - other.lef).pow(2) + (self.right - other.right).pow(2)).to_u128().unwrap_unchecked()
  }
  }


  impl Add for SafeVec {
  type Output = SafeVec;

  fn add(self, other: SafeVec) -> SafeVec {
    SafeVec {
      lef: self.lef + other.lef,
      right: self.right + other.right,
    }
  }
  }

  impl AddAssign for SafeVec {
    fn add_assign(&mut self, other: SafeVec) {
      self.lef += other.lef;
      self.right += other.right;
    }
  }

  impl Sub for SafeVec {
  type Output = SafeVec;

  fn sub(self, other: Self) -> SafeVec {
    SafeVec {
      lef: self.lef - other.lef,
      right: self.right - other.right,
    }
  }
  }

  impl SubAssign for SafeVec {
    fn sub_assign(&mut self, other: Self) {
      self.lef -= other.lef;
      self.right -= other.right;
    }
  }

  impl Mul for SafeVec {
  type Output = SafeVec;

  fn mul(self, other: Self) -> Self {
    Self {
      lef: self.lef * other.lef,
      right: self.right * other.right,
    }
  }
}

impl MulAssign for SafeVec {
fn mul_assign(&mut self, other: SafeVec) {
  self.lef *= other.lef;
  self.right *= other.right;
}
}

impl Mul<f64> for SafeVec {
type Output = SafeVec;

fn mul(self, number: f64) -> SafeVec {
  SafeVec {
    lef: (self.lef as f64 * number) as i32,
    right: (self.right as f64 * number) as i32,
  }
}
}

impl Div for SafeVec {
type Output = SafeVec;

fn div(self, other: Self) -> SafeVec {
  SafeVec {
    lef: self.lef / other.lef,
    right: self.right / other.right,
  }
}
}

impl DivAssign for SafeVec {
  fn div_assign(&mut self, other: SafeVec) {
    self.lef /= other.lef;
    self.right /= other.right;
  }
}


macro_rules! impl_div_for_safevec {
  ($($t:ty),*) => {
    $(
      impl Div<$t> for SafeVec {
          type Output = SafeVec;
          fn div(self, number: $t) -> SafeVec {
              SafeVec {
                  lef: self.lef / number as i32,
                  right: self.right / number as i32,
              }
          }
      }
    )*
  }
}

impl_div_for_safevec!(i8, i16, i32, i64, i128, isize, u8, u16, u32, u64, u128, usize);

impl Neg for SafeVec {
  type Output = SafeVec;
  fn neg(self) -> SafeVec {
    SafeVec {
        lef: -self.lef,
        right: -self.right,
    }
  }
}

impl PartialOrd for SafeVec<T> {
  fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
    Some(self.cmp(other))
  }
}

impl Ord for SafeVec {
  fn cmp(&self, other: &Self) -> Ordering {
    match self.lef.cmp(&other.lef) {
      Ordering::Equal => self.right.cmp(&other.right),
      other => other,
    }
  }
}

impl Hash for SafeVec {
  fn hash<H: Hasher>(&self, state: &mut H) {
    self.lef.hash(state);
    self.right.hash(state);
  }
}

impl fmt::Display for SafeVec {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "({}, {})", self.lef, self.right)
  }
}
