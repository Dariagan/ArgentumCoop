extends Resource
## Defines a specific Being initialization
class_name BeingKind

@export var id: String = ""

@export var race_id: String
@export var klass_id: String = "random"

@export var sex: Enums.Sex = Enums.Sex.ANY

@export var extra_health_multiplier_range: Vector2 = Vector2.ONE
@export var head_scale_range: Vector2 = Vector2.ONE
@export var body_scale_range: Vector2 = Vector2.ONE

@export var names_distribution: Dictionary = {"placeholder": 1}
@export var equipment_distribution: Dictionary
@export var loot_distribution: Dictionary
@export var raid_unit_cost: int = 100

#if none specified (array is empty), race defaults are used
#NOTA: EL DICTIONARY ES PA USAR COMO UN SET, PONER UN DUMMY VALUE
@export var blacklisted_tiles_for_spawning: Dictionary = {}

#TODO comparar la speed dictionary vs array (medir tiempo en c++)

#if used, only these can be used
#if none specified (array is empty), race defaults are used
#TODO hacer sets de tiles whitelisted comúnes para reutilizar
@export var whitelisted_tiles_for_spawning: Dictionary = {}

func serialize_rand_instance() -> Dictionary:
	var being_birth_dict:Dictionary = {
		"name": WeightedChoice.pick(names_distribution),
		"eh": randf_range(extra_health_multiplier_range.x, extra_health_multiplier_range.y),
		"race": race_id,
		"sex": sex,
		"klass": klass_id,
		"head_scale":  randf_range(head_scale_range.x, head_scale_range.y),
		"body_scale":  randf_range(body_scale_range.x, body_scale_range.y),
		#"equipment":
		#pawnkindid? para ver q tirar al morir?, o si hacer algo especial ante la muerte de este? 
	}
	return being_birth_dict

func instantiate() -> BeingReqInitData:
	var being_init_data = BeingReqInitData.new()
	being_init_data.construct(serialize_rand_instance())
	return being_init_data;
