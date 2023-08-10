extends RefCounted 
class_name BeingSpawnData

var name: String 
var sex: Enums.Sex

var race: BasicRace
var klass: Class

var followers: Array[UncontrollableRace]  

var head_scale: Vector3 = Vector3.ONE
var body_scale: Vector3 = Vector3.ONE

var head: SpriteData
var body: BodySpriteData

var chosen_extra_sprites: Array[int] = []

var level: int = 1
var extra_health_multiplier: float = 1

var faction: Faction
var starting_inventory: Dictionary

func _init(serialized_data: Dictionary = {}) -> void:
	deserialize(serialized_data)
	
# inicializar las variables con .nombre_variable = algo
	
func deserialize(serialized_data: Dictionary) -> void:

	var returned

	returned = handle_key("name", serialized_data)
	if returned: name = returned; returned = null
	
	returned = handle_key("extra_health_multiplier", serialized_data)
	if returned: extra_health_multiplier = returned; returned = null
		
	returned = handle_key("level", serialized_data)
	if returned: level = returned; returned = null
		
	var race_id: String = serialized_data["race"]
	if race_id.begins_with("controllable"):
		race = handle_key("race", serialized_data, GlobalData.controllable_races)
	else:
		race = handle_key("race", serialized_data, GlobalData.uncontrollable_races)
		
	klass = handle_key("klass", serialized_data, GlobalData.classes)
		
	if serialized_data.has("followers"):
		# BUG, ARREGLAR. DICE CLASSES AHÍ. HAY Q ARREGLAR ETO
		followers = GlobalData.classes[serialized_data["followers"]]
			
	head = handle_key("head", serialized_data, race.head_sprites_datas)
			
	body = handle_key("body", serialized_data, race.body_sprites_datas) as BodySpriteData
	
	returned = handle_key("head_scale", serialized_data)
	if returned: head_scale = returned; returned = null
	
	returned = handle_key("body_scale", serialized_data)
	if returned: body_scale = returned; returned = null

	faction = GameData.factions[serialized_data["faction"]]

func serialize() -> Dictionary:
	var dict: Dictionary = {}
	dict["name"] = name
	dict["sex"] = sex
	dict["race"] = race.id
	dict["klass"] = klass.id
	#dict["followers"] = get_array_of_ids(followers)
	dict["head_scale"] = head_scale
	dict["body_scale"] = body_scale
	dict["head"] = head.id
	dict["body"] = body.id
	dict["level"] = level
	dict["extra_health_multiplier"] = extra_health_multiplier
	dict["faction"] = faction.id
	return dict
	
func _construct_from_saved_starter_character(starter_character: Resource) -> void:
	pass

func get_array_of_ids(array_of_objects: Array) ->  Array:
	var array_ids: Array = []
	for o in array_of_objects:
		array_ids.push_back(o.id)
	return array_ids

func handle_key(key: String, serialized_data: Dictionary, data_structure = null):
	if serialized_data.has(key):
		if data_structure is Dictionary and data_structure.keys().size() > 0:
			if not (serialized_data[key] as String).ends_with("random"):
				return data_structure[serialized_data[key]]
			else:
				return get_random(data_structure.values())
		elif data_structure is Array and data_structure.size() > 0:
			for item in data_structure:
				if serialized_data[key] == item.id:
					return item
			return get_random(data_structure)
		else: 
			return serialized_data[key]

func get_random(list_for_random: Array):
	if list_for_random.size() > 0: return list_for_random[randi() % list_for_random.size()]

