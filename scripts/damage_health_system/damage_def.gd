extends Resource
class_name DamageDef

enum Mobility { NONE, MOBILIZES, CONFUSES, IMMOBILIZES, PARALYZES }
enum Invisibility { NONE, REMOVES, GIVES }

# se aplica después de que se hayan aplicado armor reductions. la armor actual del atacado resta damage. 
# la damage que quede tras la resta se aplica sobre el body del character
@export var damages: Dictionary = {"sharp": 0, "blunt": 0, "fire": 0, "magic": 0}


# reducción de armor (no produce daño)
@export_category("Armor Reduction")

@export var armor_reductions: Dictionary = {"sharp": 0, "blunt": 0, "fire": 0, "magic": 0}

@export_category("Effects")

@export var visibility_effect: Invisibility
@export var visibility_effect_duration: float

@export var mobility_effect: Mobility
@export var mobility_effect_duration: float

@export_range(-1, 1) var blindness: float

# can be negative or positive
@export_range(-15, 15) var speed: float

#por ejemplo, dragones
@export var damage_multipliers_vs_races: Dictionary

@export_category("Lasting damage")

#debería ser small
@export var venom: float # se podria hacer cada ailment un recurso

@export_range(0, 1) var venom_chance: float


@export var poison_duration: float

@export_range(0, 1) var wound_infection_chance_increase: float



