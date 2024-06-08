use std::thread::current;

pub use crate::tiling::TileSelection;
use crate::uns_vec::UnsVec;
use crate::world_matrix::*;
pub use crate::{world_matrix::WorldMatrix};
pub use godot::builtin::Dictionary;
use enum_primitive_derive::Primitive;
use godot::obj::{Base, Gd, GdRef};
use godot::prelude::*;

#[derive(GodotConvert, Var, Export, Primitive)]
#[godot(via = i64)]
pub enum FormGenEnum {
    FracturedFormationGenerator = 0,
}


pub trait IFormationGenerator {
    fn generate(
        world: WorldMatrix,
        origin: UnsVec,
        size: UnsVec,
        tile_selection: Gd<TileSelection>,
        seed: i32,
        data: Dictionary,
    ) -> WorldMatrix;
}

pub fn get_border_closeness_factor(
    coords: &UnsVec,
    world_size: &UnsVec,
    power: Option<f64>,
) -> f64 {
    let power = power.unwrap_or(3.0);

    let horizontal_border_closeness: f64 = ((coords.lef as f64 - world_size.lef as f64 / 2.0)
        / (world_size.lef as f64 / 2.0))
        .abs()
        .powf(power);
    let vertical_border_closeness: f64 = ((coords.right as f64 - world_size.right as f64 / 2.0)
        / (world_size.right as f64 / 2.0))
        .abs()
        .powf(power);

    horizontal_border_closeness.max(vertical_border_closeness)
}

pub fn overwrite_formation_tile(world_matrix: *mut WorldMatrix, (origin, relative): (UnsVec, UnsVec), nid_or_dist: NidOrDist, instantiation_data: Option<Dictionary>){
    unsafe{
        // z level de tiles falta, volver nidorniddistribution Gd<tile>distribution?
                 
        match nid_or_dist {
            //EL ESTADO HAY Q ASIGNARLO EN OTRO LADO, SINO CADA CASILLA VA A OCUPAR MUCHO MÁS DE 2 BYTES Y EL USO DE MEMORIA SE VUELVE EXCESIVO.
            //PARA EL ESTADO USAR UN DICT <COORD,STATE> EN GODOT, Q SE ASIGNE DESDE ACA CON UNA SIGNAL
            NidOrDist::Nid((nid, z_level)) => {(*world_matrix).overwrite_tile(nid, origin+relative, z_level);},
            NidOrDist::Dist(dist) => {
                let (nid,z_level) = dist.sample();
                (*world_matrix).overwrite_tile(nid, origin+relative, z_level);
            }

        }
        return;
        
        todo!()
    }
}

pub fn generate_stateful_instance(world_matrix: *mut WorldMatrix, (origin, relative): (UnsVec, UnsVec), nid: NidOrDist, instantiation_data: Dictionary){
    todo!()
    //llamar señal o algo
}
//UnsVec is a Vec2D whose components are lef(x) and right(y)
struct FormationCoordsIterator{

    current: UnsVec,
    limits: (u32, u32),
}

impl FormationCoordsIterator{

    pub fn new(size: UnsVec, thread_i: u32, n_threads: u32) -> Self {
        let horizontal_range: UnsVec = UnsVec { 
            lef: (thread_i*size.lef)/n_threads, 
            right: (thread_i*size.right)/n_threads
        };
        Self{
            current: UnsVec {lef: horizontal_range.lef, right: 0}, 
            limits: (horizontal_range.right, size.right)
        }
    }
}
//creo que estos ifs lo hacen más lento que un for loop crudo, va a haber que hacer un benchmark comparativo
impl Iterator for FormationCoordsIterator{
    type Item = UnsVec;

    fn next(&mut self) -> Option<Self::Item> {
        
        if self.current.lef >= self.limits.0 {
            return None;
        }
        let result = self.current.clone();

        if self.current.right + 1 >= self.limits.1 {
            self.current.lef += 1;
            self.current.right = 0;
        } else {
            self.current.right += 1;
        }

        Some(result)
    }
}


pub struct SendPtr<T>(pub *const T);
unsafe impl<T> Send for SendPtr<T> where T: Send {}
unsafe impl<T> Sync for SendPtr<T> where T: Sync {}

impl<T> Clone for SendPtr<T> {
    fn clone(&self) -> Self {
        SendPtr(self.0)
    }
}
impl<T> Copy for SendPtr<T> {}
struct SendMutPtr<T>(*mut T);
unsafe impl<T> Send for SendMutPtr<T> where T: Send {}
unsafe impl<T> Sync for SendMutPtr<T> where T: Sync {}

impl<T> Clone for SendMutPtr<T> {
    fn clone(&self) -> Self {
        SendMutPtr(self.0)
    }
}
impl<T> Copy for SendMutPtr<T> {}
macro_rules! make_ptr {
    ($ref:expr) => {
        SendPtr {
            0: $ref as *const _,
        }
    };
}


macro_rules! make_mut_ptr {
    ($ref:expr) => {
        SendMutPtr {
            0: $ref as *mut _,
        }
    };
}

pub(crate) use make_ptr;  
pub(crate) use make_mut_ptr;  