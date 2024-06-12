pub struct SendPtr<T>(pub *const T);
unsafe impl<T> Send for SendPtr<T> where T: Send {} unsafe impl<T> Sync for SendPtr<T> where T: Sync {}
impl<T> Clone for SendPtr<T> {
    fn clone(&self) -> Self {
        SendPtr(self.0)
    }
}
impl<T> Copy for SendPtr<T> {}
pub struct SendMutPtr<T>(pub *mut T);
unsafe impl<T> Send for SendMutPtr<T> where T: Send {} unsafe impl<T> Sync for SendMutPtr<T> where T: Sync {}
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
}pub(crate) use make_ptr;  
macro_rules! make_mut_ptr {
    ($ref:expr) => {
        SendMutPtr {
            0: $ref as *mut _,
        }
    };
}pub(crate) use make_mut_ptr;  
pub trait DerefPtr {
    type Target;
    unsafe fn drf(&self) -> &Self::Target;
}
pub trait DerefMutPtr {
    type Target;
    unsafe fn drf(&mut self) -> &mut Self::Target;
}
impl<T> DerefPtr for SendPtr<T> {
    type Target = T;
    unsafe fn drf(&self) -> &T {
        &*self.0
    }
}
impl<T> DerefMutPtr for SendMutPtr<T> {
    type Target = T;
    unsafe fn drf(&mut self) -> &mut T {
        &mut *self.0
    }
}