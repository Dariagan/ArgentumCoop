extends Resource
## Defines a specific Being initialization
class_name BeingKindDef

@export var id: String = ""

@export var race: BasicRace
@export var klass: Klass

@export var extra_health_multiplier_range: Vector2
@export var head_scale_range: Vector2
@export var body_scale_range: Vector2

@export var names_distribution: Dictionary
@export var equipment_distribution: Dictionary
@export var loot_distribution: Dictionary
@export var raid_unit_cost: int

#if none specified (array is empty), race defaults are used
#NOTA: EL DICTIONARY ES PA USAR COMO UN SET, PONER UN DUMMY VALUE
@export var blacklisted_tiles_for_spawning: Dictionary = {}

#if used, only these can be used
#if none specified (array is empty), race defaults are used
#TODO hacer sets de tiles whitelisted comúnes para reutilizar
@export var whitelisted_tiles_for_spawning: Dictionary = {}
