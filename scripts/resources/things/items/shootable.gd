extends Resource
class_name Shootable


@export var current_magical_multipliers_apply: bool
@export var current_ranged_multipliers_apply: bool
@export var damage_def: DamageDef #el damage se suma al de la flecha

# q saque la ammo en órden izq-der del quiver
# si es null, no usa ammo
@export var ammunition_category: AmmunitionCategory

@export var cooldown: float
