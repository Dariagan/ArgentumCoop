extends Resource
class_name WearStats

# Doesn't do anything on it's own. It's meant to be multiplied by the armors array
@export_range(0,20) var base_defense: int = 0

@export var damage_type_reduction_multipliers: Dictionary = {"sharp": 0, "blunt": 0, "fire": 0, "magic": 0}

@export_range(-20,20) var temperature_offset: int = 0

@export var worn_encumberance_multiplier: float = 0


