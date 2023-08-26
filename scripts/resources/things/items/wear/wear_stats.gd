extends Resource
class_name WearStats

#NOT a multiplier
@export var damage_reductions: Dictionary = {"sharp": 0, "blunt": 0, "fire": 0, "magic": 0}

@export_range(-20,20) var temperature_offset: int = 0

@export var worn_encumberance_multiplier: float = 0
