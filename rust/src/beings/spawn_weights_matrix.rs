use crate::utils::matrix::DownScalingMatrix;

use super::BeingUnid;



struct SpawnWeight {
  weight: u16,
  bunid: BeingUnid
}

pub struct SpawnWeightsMatrix{
  matrix: DownScalingMatrix<Vec<SpawnWeight>>
}