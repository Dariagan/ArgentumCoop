
use rand::Rng;
use std::collections::HashMap;

use crate::beings::{spawn_weights_matrix, BeingGenTemplIdAndFac};

// Assuming `BeingKindStrId` is your key type and `SpawnWeight` is the weight type.
pub fn sample_from_weighted_map(
    spawn_weights: &HashMap<BeingGenTemplIdAndFac, spawn_weights_matrix::SpawnWeight>,
) -> Option<BeingGenTemplIdAndFac> {
    let total_weight: u16 = spawn_weights.values().map(|weight| weight.0).sum();
    
    if total_weight == 0 {return None; }
    
    let mut rng = rand::rng();
    let random_value: u16 = rng.random_range(0..total_weight);
    
    let mut accumulated_weight = 0;
    
    for (key, weight) in spawn_weights.iter() {
        accumulated_weight += weight.0;
        
        // If the accumulated weight exceeds the random value, return the key.
        if accumulated_weight >= random_value {
            return Some(key.clone()); // Assuming `BeingKindStrId` implements `Clone`.
        }
    }
    
    None
}
