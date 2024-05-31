use std::cmp::Ordering;
use std::fmt;
use std::hash::{Hash, Hasher};
use std::ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Neg, Sub, SubAssign};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct SafeVec {
    pub lef: i32,
    pub right: i32,
}

impl SafeVec {
    pub fn new(lef: i32, right: i32) -> Self {
        SafeVec { lef, right }
    }

    pub fn add_assign_lef(&mut self, other: &Self) {
        self.lef += other.lef;
    }

    pub fn add_assign_right(&mut self, other: &Self) {
        self.right += other.right;
    }

    pub fn sum_lef(&self, i: i32) -> Self {
        SafeVec {
            lef: self.lef + i,
            right: self.right,
        }
    }

    pub fn sum_right(&self, j: i32) -> Self {
        SafeVec {
            lef: self.lef,
            right: self.right + j,
        }
    }

    pub fn compare_lef(&self, other: &Self) -> Ordering {
        self.lef.cmp(&other.lef)
    }

    pub fn compare_right(&self, other: &Self) -> Ordering {
        self.right.cmp(&other.right)
    }

    pub fn is_strictly_smaller_than(&self, other: &Self) -> bool {
        self.lef < other.lef && self.right < other.right
    }

    pub fn is_strictly_bigger_than(&self, other: &Self) -> bool {
        self.lef > other.lef && self.right > other.right
    }

    pub fn is_any_comp_negative(&self) -> bool {
        self.lef < 0 || self.right < 0
    }

    pub fn is_non_negative(&self) -> bool {
        self.lef >= 0 && self.right >= 0
    }

    pub fn is_strictly_positive(&self) -> bool {
        self.lef > 0 && self.right > 0
    }

    pub fn c_str(&self) -> String {
        format!("({}, {})", self.lef, self.right)
    }

    pub fn flat_index(&self, size: &SafeVec) -> usize{
        (self.lef*size.lef + self.right) as usize
    }

    pub fn length(&self) -> f64 {
        self.distance_to(&SafeVec { lef: 0, right: 0 })
    }

    pub fn area(&self) -> usize {
        (self.lef * self.right).abs() as usize
    }

    pub fn distance_to(&self, other: &Self) -> f64 {
        (((self.lef - other.lef).pow(2) + (self.right - other.right).pow(2)) as f64).sqrt()
    }

    pub unsafe fn distance_squared_to(&self, other: &Self) -> usize {
        ((self.lef - other.lef).pow(2) + (self.right - other.right).pow(2)) as usize
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

    fn sub(self, other: SafeVec) -> SafeVec {
        SafeVec {
            lef: self.lef - other.lef,
            right: self.right - other.right,
        }
    }
}

impl SubAssign for SafeVec {
    fn sub_assign(&mut self, other: SafeVec) {
        self.lef -= other.lef;
        self.right -= other.right;
    }
}

impl Mul for SafeVec {
    type Output = SafeVec;

    fn mul(self, other: SafeVec) -> SafeVec {
        SafeVec {
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

    fn div(self, other: SafeVec) -> SafeVec {
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

impl Div<f64> for SafeVec {
    type Output = SafeVec;
    fn div(self, number: f64) -> SafeVec {
        SafeVec {
            lef: (self.lef as f64 / number) as i32,
            right: (self.right as f64 / number) as i32,
        }
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

impl PartialOrd for SafeVec {
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
