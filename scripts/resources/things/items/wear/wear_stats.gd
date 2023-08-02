extends Resource
class_name WearStats

# Doesn't do anything on it's own. It's meant to be multiplied by the armors array
@export_range(0,20) var base_armor: int = 0

@export var armors: Array[AttackTypeMultiplier] 

@export_range(-20,20) var temperature_offset: int = 0


