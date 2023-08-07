extends Resource #dejar como resource para q sea guardable el initial character, 
#pero va a haber q hacer otra clase q chequee q no se metan stats chiteados o incompatibles
class_name BeingSpawnData

@export var name: String = " "
@export var sex: Enums.Sex
@export var race: BasicRace
@export var klass: Class


@export var followers: Array[UncontrollableRace]  

@export var size_multiplier: Vector2 = Vector2(1, 1) #width, height

@export var head: int = 0
@export var body: int = 0

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
	dict["size_multiplier"] = size_multiplier
	dict["head"] = head
	dict["body"] = body
	
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
	
	if serialized_data.has("size_multiplier"):
		size_multiplier = serialized_data["size_multiplier"]
		
	if serialized_data.has("klass"):
		klass = GlobalGameData.classes[serialized_data["klass"]]
		
	if serialized_data.has("followers"):
		followers = GlobalGameData.classes[serialized_data["followers"]]
	
	if serialized_data.has("head"):
		head = serialized_data["head"]
	if serialized_data.has("body"):	
		body = serialized_data["body"]	
	
	
	#faction = serialized_data["faction"]
	




