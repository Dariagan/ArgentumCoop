extends Resource
class_name CombatMultipliers

@export_category("General")
@export var health: float = 1
@export var speed: float = 1
@export var strength: float = 1
@export var stamina: float = 1
@export var sneaking_proficiency: float = 1

@export var incoming_damage_types: Dictionary = {"sharp": 1, "blunt": 1, "fire": 1, "magic": 1}
 #key: id of the damage type. value: value the incoming damage of the specified tyoe is multiplied by
@export var war_cry_strength: float = 1

@export var melee_multipliers: MeleeMultipliers

@export var ranged_multipliers: RangedMultipliers

@export var magic_multipliers: MagicMultipliers
