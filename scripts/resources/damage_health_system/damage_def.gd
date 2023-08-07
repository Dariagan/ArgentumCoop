extends Resource
class_name DamageDef

enum Mobility { NONE, MOBILIZES, CONFUSES, IMMOBILIZES, PARALYZES }
enum Invisibility { NONE, REMOVES, GIVES }

@export_category("Damage Proportions")

@export var damage_ratios: Array[AttackTypeMultiplier]

#hacer q soo se exporte un array con todas las reductions opcionales?
@export_category("Armor Reduction")

@export var armor_reduction_ratios: Array[AttackTypeMultiplier]

@export_category("Effects")

@export var visibility_effect: Invisibility
@export var visibility_effect_duration: float

@export var mobility_effect: Mobility
@export var mobility_effect_duration: float

@export_range(-1, 1) var blindness: float

# can be negative or positive
@export_range(-15, 15) var speed: float

@export var damage_multipliers_vs_races: Dictionary

@export_category("Lasting damage")

#debería ser small
@export var venom: float # se podria hacer cada ailment un recurso

@export_range(0, 1) var venom_chance: float


@export var poison_duration: float

@export_range(0, 1) var wound_infection_chance_increase: float



