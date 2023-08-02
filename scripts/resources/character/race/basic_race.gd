extends Resource
class_name BasicRace


@export var id: String = "vanilla_"
@export var name: String
@export_multiline var description: String 

@export_range(0, 3) var hunger_rate: float = 1

@export_category("Combat Multipliers")

@export var health_multiplier: float = 1
@export var max_speed_multiplier: float = 1

@export var strength_multiplier: float = 1
#TODO ponerle male y female a cada variable?

@export var male_can_vent: bool = false
@export var female_can_vent: bool = false

@export var damage_reductions: Array[AttackTypeMultiplier]

@export_range(0, 3) var sneak_proficiency: float = 1

#export array de tupla biome_id: string preferrance: float


@export_category("Population")
#1 -> 100% of pawns are male 0 _> all pawns are female
@export_range(0, 1) var males_females_ratio: float = 0.5
