extends RefCounted 
#BeingRequiredInitializationData
class_name BeingReqInitData
#se diferencia de un pawnkinddef en q el pawnkinddef puede incluir rangos de aleatoriedad/sets weighteados configurables para ciertos atributos/variables
#dentro de ahí, (money, possible loot, health, possible names). y en pawnkinddef no se especifica la faction. en esto sí, para saber en cual meter al being
#
#esto contiene solo valores deterministas para el spawning de un individuo (excepto por la randomización de la cara y cuerpo si no se especifican)

var name: String 
var head_scale: Vector3 = Vector3.ONE
var body_scale: Vector3 = Vector3.ONE

var sprite_head: SpriteData
var sprite_body: BodySpriteData

var chosen_extra_sprites: Array[int] = []

var extra_health_multiplier: float = 1

var internal_state: BeingInternalState
	
func construct(being_birth_dict: Dictionary) -> void:
	assert(being_birth_dict != null && being_birth_dict != {})
	var sex: Enums.Sex
	var race: BasicRace
	var klass: Klass
	var faction: Faction
	var followers: Array[UncontrollableRace]  
	var result
	result = handle_key("name", being_birth_dict)
	if result: name = result; result = null
	
	result = handle_key("eh", being_birth_dict)
	if result: extra_health_multiplier = result; result = null
		
	#result = handle_key("level", being_birth_dict)
	#if result: level = result; result = null
		
	var race_id: String = being_birth_dict["race"]
	if race_id.begins_with("controllable"):
		race = handle_key("race", being_birth_dict, GlobalData.controllable_races)
	elif race_id.begins_with("controllable"):
		race = handle_key("race", being_birth_dict, GlobalData.uncontrollable_races)
	else:
		push_error("not a valid race id")
		
	klass = handle_key("klass", being_birth_dict, GlobalData.klasses)
	
	faction = handle_key("fac", being_birth_dict, GameData.factions)
		
	if being_birth_dict.has("followers"):
		# BUG, ARREGLAR. DICE  AHÍ. HAY Q ARREGLAR ETO
		followers = GlobalData.klasses[being_birth_dict["followers"]]
			
	sprite_head = handle_key("head", being_birth_dict, race.head_sprites_datas)
			
	sprite_body = handle_key("body", being_birth_dict, race.body_sprites_datas) as BodySpriteData
	
	result = handle_key("head_scale", being_birth_dict)
	if result: head_scale = result; result = null
	
	result = handle_key("body_scale", being_birth_dict)
	if result: body_scale = result; result = null
	
	var sex_value = being_birth_dict["sex"]
	
	if sex_value is String or sex_value == Enums.Sex.ANY:
		var sex_probs: Dictionary = {
			Enums.Sex.MALE: race.males_ratio,
			Enums.Sex.FEMALE: 1 - race.males_ratio
		}
		sex = WeightedChoice.pick(sex_probs)
	elif sex_value is Enums.Sex:
		sex = sex_value

	internal_state = BeingInternalState.new()
	internal_state.construct_locally(sex, race, faction, null, klass)

# TODO
func construct_from_serialized(serialized_being_spawn_data: Dictionary) -> void:
	pass

func serialize() -> Dictionary:
	var dict: Dictionary = {
		"name": name,
		"head_scale": head_scale,
		"body_scale": body_scale,
		"head": sprite_head.id,
		"body": sprite_body.id,
		"state": internal_state.serialize(),
		"eh": extra_health_multiplier
		#extra_stats_multiplier
	}
	#dict["followers"] = get_array_of_ids(followers)
	return dict

#NO IMPLEMENTAR ESTA FUNCIÓN, PERO IMPLEMENTAR LA IDEA DE CARGAR STARTER CHARACTERS ASÍ NO PERDÉS TIEMPO RE-CREÁNDOLOS EN CADA LOBBY
func _construct_from_saved_starter_character(starter_character: Resource) -> void:
	pass

func get_array_of_ids(array_of_objects: Array) ->  Array:
	var array_ids: Array = []
	for o in array_of_objects:
		array_ids.push_back(o.id)
	return array_ids

func handle_key(key: String, being_birth_dict: Dictionary, data_structure = null):
	if being_birth_dict.has(key):
		if data_structure is Dictionary and data_structure.keys().size() > 0:
			if not (being_birth_dict[key] as String).ends_with("random"):
				return data_structure[being_birth_dict[key]]
			else:
				return get_random(data_structure.values())
		elif data_structure is Array and data_structure.size() > 0:
			for item in data_structure:
				if being_birth_dict[key] == item.id:
					return item
			return get_random(data_structure)
		else: 
			return being_birth_dict[key]
	else:
		print("key %s not found" % key)

func get_random(list_for_random: Array):
	if list_for_random.size() > 0: return list_for_random[randi() % list_for_random.size()]

