extends Resource
class_name WearStats

#NOT a multiplier
@export var damage_reductions: Dictionary[Enu.PierceType, float] = {Enu.PierceType.SHARP: 0, Enu.PierceType.BLUNT: 0, Enu.PierceType.FIRE: 0, Enu.PierceType.MAGIC: 0, Enu.PierceType.FROST: 0}

@export_range(-20,20) var temperature_offset: int = 0

@export var worn_encumberance_multiplier: float = 0
