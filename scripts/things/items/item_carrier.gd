extends Resource
class_name ItemCarrier

@export var holdable_item_types: Array[ItemData] 
@export var max_contained_real_encumberance: float = 0
@export var max_contained_real_weight: float = 0
@export var weight_multiplier: float = 0
@export var encumberance_multiplier: float = 0
@export var encumberance_reducion_on_worn_only: bool = true

var held_items: Array

#usar duplicate
