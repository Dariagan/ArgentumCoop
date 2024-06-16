extends RefCounted 
#BeingRequiredInitializationData
class_name BeingStatePreIniter
#se diferencia de un beingkind en q el beingkind puede incluir rangos de aleatoriedad/sets weighteados configurables para ciertos atributos/variables
#dentro de ahí, (money, possible loot, health, possible names). y en beingkind no se especifica la faction. en esto sí, para saber en cual meter al being
#
#esto devuelve solo valores absolutamente especificos y deterministas para el
# spawning de un individuo específico con caracteristicas especificadas
# (excepto por la randomización de la cara y cuerpo si no se especifican)

var name: String 
var head_scale: Vector3 = Vector3.ONE
var body_scale: Vector3 = Vector3.ONE

var sprite_head: SpriteData
var sprite_body: BodySpriteData

var chosen_extra_sprites: Array[int] = []

var extra_health_multiplier: float = 1

var internal_state: BeingInternalState

#only the key literals
const KEYS: Dictionary = { 
	NAME = &"name",
	SEX = &"sex",
	RACE = &"race",
	FACTION = &"faction",
	KLASS = &"klass",
	FOLLOWERS = &"followers",
	HEAD_SCALE = &"head_scale",
	BODY_SCALE = &"body_scale",
	HEAD = &"head",
	BODY = &"body",
	EXTRA_HEALTH_MULTI = &"eh",
	BEINGKIND = &"beingkind",
	INTERNAL_STATE = &"istate",
}

func construct(being_birth_dict: Dictionary) -> void:
	assert(being_birth_dict != null && being_birth_dict != {})
	
	#region are constructed inside internal_state
	var sex: Constants.Sex
	var race: BasicRace
	var klass: Klass
	var faction: Faction
	var followers: Array[UncontrollableRace] 
	var beingkind: BeingKind 
	#endregion
	var result
	result = handle_key(KEYS.NAME, being_birth_dict)
	if result: name = result; result = null
	
	result = handle_key(KEYS.EXTRA_HEALTH_MULTI, being_birth_dict)
	if result: extra_health_multiplier = result; result = null
		
	#result = handle_key("level", being_birth_dict)
	#if result: level = result; result = null
		
	var race_id: StringName = being_birth_dict[KEYS.RACE]
	
	if race_id == &"controllable_random":
		being_birth_dict[KEYS.RACE] = &"random"
		race = handle_key(KEYS.RACE, being_birth_dict, GlobalData.controllable_races)
	elif race_id == &"uncontrollable_random":
		being_birth_dict[KEYS.RACE] = &"random"
		race = handle_key(KEYS.RACE, being_birth_dict, GlobalData.uncontrollable_races)	
	elif GlobalData.races[race_id] is ControllableRace:
		race = handle_key(KEYS.RACE, being_birth_dict, GlobalData.controllable_races)
	elif GlobalData.races[race_id] is UncontrollableRace:
		race = handle_key(KEYS.RACE, being_birth_dict, GlobalData.uncontrollable_races)
	else:
		push_error("not a valid race id")
		
	klass = handle_key(KEYS.KLASS, being_birth_dict, GlobalData.klasses)
	
	faction = handle_key(KEYS.FACTION, being_birth_dict, GameData.factions)
		
	if being_birth_dict.has(KEYS.FOLLOWERS):
		# BUG, ARREGLAR. DICE  AHÍ. HAY Q ARREGLAR ETO
		followers = GlobalData.klasses[being_birth_dict[KEYS.FOLLOWERS]]
			
	sprite_head = handle_key(KEYS.HEAD, being_birth_dict, race.head_sprites_datas)
			
	sprite_body = handle_key(KEYS.BODY, being_birth_dict, race.body_sprites_datas) as BodySpriteData
	
	result = handle_key(KEYS.HEAD_SCALE, being_birth_dict)
	if result: head_scale = result; result = null
	
	result = handle_key(KEYS.BODY_SCALE, being_birth_dict)
	if result: body_scale = result; result = null
	
	var sex_value = being_birth_dict[KEYS.SEX]
	
	if sex_value is StringName or sex_value == Constants.Sex.ANY:
		var sex_probs: Dictionary = {
			Constants.Sex.MALE: race.males_ratio,
			Constants.Sex.FEMALE: 1 - race.males_ratio
		}
		sex = WeightedChoice.pick(sex_probs)
	elif sex_value is Constants.Sex:
		sex = sex_value
	else:
		push_error("invalid type for \"sex\" entry in birth dict")
		
	if being_birth_dict.has(KEYS.BEINGKIND):
		beingkind = handle_key(KEYS.BEINGKIND, being_birth_dict, GlobalData.beingkinds)

	assert(sex && race && faction)
	internal_state = BeingInternalState.new()
	internal_state.construct_for_posterior_serialization(sex, race, faction, null, klass, beingkind)

# TODO
func construct_from_serialized(serialized_being_spawn_data: Dictionary) -> void:
	pass
	
func serialize_being_internal_state() -> Dictionary:
	return internal_state.serialize()

func serialize() -> Dictionary:
	var dict: Dictionary = {
		KEYS.NAME: name,
		KEYS.HEAD_SCALE: head_scale,
		KEYS.BODY_SCALE: body_scale,
		KEYS.HEAD: sprite_head.id,
		KEYS.BODY: sprite_body.id,
		KEYS.INTERNAL_STATE: internal_state.serialize(),
		KEYS.EXTRA_HEALTH_MULTI: extra_health_multiplier,
		#extra_stats_multiplier,
	}
	#dict[KEYS.FOLLOWERS] = get_array_of_ids(followers)
	return dict

#NO IMPLEMENTAR ESTA FUNCIÓN, PERO IMPLEMENTAR LA IDEA DE CARGAR STARTER CHARACTERS ASÍ NO PERDÉS TIEMPO RE-CREÁNDOLOS EN CADA LOBBY
func _construct_from_saved_starter_character(starter_character: Resource) -> void:
	pass

func get_array_of_ids(array_of_objects: Array) ->  Array:
	var array_ids: Array = []
	for o in array_of_objects:
		array_ids.push_back(o.id)
	return array_ids

func handle_key(key: StringName, being_birth_dict: Dictionary, data_structure = null):
	if being_birth_dict.has(key):
		if data_structure is Dictionary and data_structure.keys().size() > 0:
			if being_birth_dict[key] != &"random":
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
		push_warning("key %s not found" % key)

func get_random(list_for_random: Array):
	if list_for_random.size() > 0: return list_for_random[randi() % list_for_random.size()]
