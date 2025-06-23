extends Resource
class_name WearStats

#NOT a multiplier
@export var damage_reductions: Dictionary[Enum.PierceType, float] = {Enum.PierceType.SHARP: 0, Enum.PierceType.BLUNT: 0, Enum.PierceType.FIRE: 0, Enum.PierceType.MAGIC: 0, Enum.PierceType.FROST: 0}

@export_range(-20,20) var temperature_offset: int = 0

@export var worn_encumberance_multiplier: float = 0
