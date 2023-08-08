extends Resource #dejar como resource para q sea guardable el initial character, 
#pero va a haber q hacer otra clase q chequee q no se metan stats chiteados o incompatibles
class_name BeingSpawnData

@export var name: String 
@export var sex: Enums.Sex
@export var race: BasicRace
@export var klass: Class


@export var followers: Array[UncontrollableRace]  

@export var head_scale: Vector3 = Vector3.ONE
@export var body_scale: Vector3 = Vector3.ONE

@export var head_i: int = -1
@export var body_i: int = 0

@export var chosen_extra_sprites: Array[int] = []

@export var level: int = 1
@export var extra_health_multiplier: float = 1
@export var faction: Faction
@export var starting_inventory: Dictionary

func serialize() -> Dictionary:
	var dict: Dictionary = {}
	dict["name"] = name
	dict["sex"] = sex
	dict["race"] = race.id
	dict["klass"] = klass.id
	dict["followers"] = get_array_of_ids(followers)
	dict["head_scale"] = head_scale
	dict["body_scale"] = body_scale
	dict["head_i"] = head_i
	dict["body_i"] = body_i
	dict["level"] = level
	dict["extra_health_multiplier"] = extra_health_multiplier
	dict["faction"] = faction.id
	return dict

func get_array_of_ids(array_of_objects: Array) ->  Array:
	var array_ids: Array = []
	for o in array_of_objects:
		array_ids.push_back(o.id)
	return array_ids

func _init(serialized_data: Dictionary = {}) -> void:
	deserialize(serialized_data)
	
# inicializar las variables con .nombre_variable = algo
	
func deserialize(serialized_data: Dictionary) -> void:
	
	if serialized_data.has("name"):
		name = serialized_data["name"]
	
	if serialized_data.has("extra_health_multiplier"):
		extra_health_multiplier = serialized_data["extra_health_multiplier"]
	if serialized_data.has("level"):
		level = serialized_data["level"]
		
	var race_id: StringName = serialized_data["race"]
	if race_id.begins_with("controllable_"):
		race = GlobalGameData.controllable_races[race_id]
	elif race_id.begins_with("uncontrollable_"):
		race = GlobalGameData.uncontrollable_races[race_id]
	
	if serialized_data.has("head_scale"):
		head_scale = serialized_data["head_scale"]
	
	if serialized_data.has("body_scale"):
		body_scale = serialized_data["body_scale"]
		
	if serialized_data.has("klass"):
		klass = GlobalGameData.classes[serialized_data["klass"]]
		
	if serialized_data.has("followers"):
		followers = GlobalGameData.classes[serialized_data["followers"]]
	
	if serialized_data.has("head_i"):
		head_i = serialized_data["head_i"]
	if serialized_data.has("body_i"):	
		body_i = serialized_data["body_i"]	
	
	
	#faction = serialized_data["faction"]
	




