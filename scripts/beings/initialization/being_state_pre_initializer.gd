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


func construct(being_birth_dict: Dictionary) -> void:
	assert(being_birth_dict != null && being_birth_dict != {})
	
	#region are constructed inside internal_state
	var sex: Enums.Sex
	var race: BasicRace
	var klass: Klass
	var faction: Faction
	var followers: Array[UncontrollableRace] 
	var beingkind: BeingKind 
	#endregion
	var result
	
	
	result = handle_key(Keys.HEALTH_MULTIP, being_birth_dict)
	if result: extra_health_multiplier = result; result = null
		
	#result = handle_key("level", being_birth_dict)
	#if result: level = result; result = null
		
	var race_id: StringName = being_birth_dict[Keys.RACE]
	
	if race_id == &"controllable_random":
		being_birth_dict[Keys.RACE] = &"random"
		race = handle_key(Keys.RACE, being_birth_dict, Global.controllable_races)
	elif race_id == &"uncontrollable_random":
		being_birth_dict[Keys.RACE] = &"random"
		race = handle_key(Keys.RACE, being_birth_dict, Global.uncontrollable_races)	
	elif Global.races[race_id] is ControllableRace:
		race = handle_key(Keys.RACE, being_birth_dict, Global.controllable_races)
	elif Global.races[race_id] is UncontrollableRace:
		race = handle_key(Keys.RACE, being_birth_dict, Global.uncontrollable_races)
	else:
		push_error("not a valid race id")
		
	if being_birth_dict[Keys.NAME] != &"random":
		name = being_birth_dict[Keys.NAME]
	elif race.default_being_names.size() > 0:
		name = race.default_being_names.pick_random()
	else:
		name = "nameless"
		
	klass = handle_key(Keys.KLASS, being_birth_dict, Global.klasses)
	
	faction = handle_key(Keys.FACTION, being_birth_dict, GameData.factions)
		
	if being_birth_dict.has(Keys.FOLLOWERS):
		# BUG, ARREGLAR. DICE klasses AHÍ. HAY Q ARREGLAR ETO
		followers = Global.klasses[being_birth_dict[Keys.FOLLOWERS]]
			
	sprite_head = handle_key(Keys.HEAD, being_birth_dict, race.head_sprites_datas)
			
	sprite_body = handle_key(Keys.BODY, being_birth_dict, race.body_sprites_datas) as BodySpriteData
	
	result = handle_key(Keys.HEAD_SCALE, being_birth_dict)
	if result: head_scale = result; result = null
	
	result = handle_key(Keys.BODY_SCALE, being_birth_dict)
	if result: body_scale = result; result = null
	
	var sex_value = being_birth_dict[Keys.SEX]
	
	if sex_value is StringName or sex_value == Enums.Sex.ANY:
		var sex_probs: Dictionary = {
			Enums.Sex.MALE: race.males_ratio,
			Enums.Sex.FEMALE: 1 - race.males_ratio
		}
		sex = WeightedChoice.pick(sex_probs)
	elif sex_value is Enums.Sex:
		sex = sex_value
	else:
		push_error("invalid type for \"sex\" entry in birth dict")
		
	if being_birth_dict.has(Keys.BEINGKIND):
		beingkind = handle_key(Keys.BEINGKIND, being_birth_dict, Global.beingkinds)

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
		Keys.NAME: name,
		Keys.HEAD_SCALE: head_scale,
		Keys.BODY_SCALE: body_scale,
		Keys.HEAD: sprite_head.id,
		Keys.BODY: sprite_body.id,
		Keys.INTERNAL_STATE: internal_state.serialize(),
		Keys.HEALTH_MULTIP: extra_health_multiplier,
		#extra_stats_multiplier,
	}
	#dict[Keys.FOLLOWERS] = get_array_of_ids(followers)
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
			if being_birth_dict[key] != &"random" && being_birth_dict[key] != "":
				return data_structure[being_birth_dict[key]]
			else:
				return data_structure.values().pick_random()
		elif data_structure is Array and data_structure.size() > 0:
			for item in data_structure:
				if being_birth_dict[key] == item.id:
					return item
			return (data_structure as Array).pick_random()
		else: 
			return being_birth_dict[key]
	else:
		push_warning("key %s not found" % key)
