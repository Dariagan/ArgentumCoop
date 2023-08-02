extends Resource
class_name Race


@export var id: String
@export var name: String
@export_multiline var description: String 

@export_range(0, 3) var hunger_rate: float = 1

@export var health_multiplier: float = 1
@export var max_speed_multiplier: float = 1

@export var strength_multiplier: float = 1
#TODO ponerle male y female a cada variable?

@export var male_can_vent: bool = false
@export var female_can_vent: bool = false

@export var damage_reductions: Array[AttackTypeMultiplier]

@export_category("Work Multipliers")

@export_range(0, 2) var global_learning_multiplier: float = 0
@export_range(0, 2) var smithing_learning_multiplier: float = 0
@export_range(0, 2) var manual_labor_multiplier: float = 0
@export_range(0, 2) var research_multiplier: float = 0

@export_range(0, 2) var trade_proficiency: float = 0
@export_range(0, 2) var sneak_proficiency: float = 0

#1 -> 100% of pawns are male 0 _> all pawns are female
@export_range(0, 1) var males_females_ratio: float = 0.5
