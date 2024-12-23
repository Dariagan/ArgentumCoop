extends RefCounted 
#BeingRequiredInitializationData
class_name BeingStatePreIniter
#se diferencia de un being_gen_template en q el being_gen_template puede incluir rangos de aleatoriedad/sets weighteados configurables para ciertos atributos/variables
#dentro de ahí, (money, possible loot, health, possible names). y en being_gen_template no se especifica la faction. en esto sí, para saber en cual meter al being
#
# esto DEVUELVE solo valores absolutamente especificos y deterministas (pos-selección aleatoria de las distribuciones especificadas) para el
# spawning de un individuo específico con caracteristicas especificadas
# (excepto por la randomización de la cara y cuerpo si no se especifican)

var mname: String 
var mhead_scale: Vector3 = Vector3.ONE; var mbody_scale: Vector3 = Vector3.ONE
var msprite_head: SpriteData; var msprite_body: BodySpriteData
var mchosen_extra_sprites: Array[int] = []
var mextra_health_multiplier: float = 1
var mistate: BeingInternalState; var mfollowers: Array[BeingGenTemplate] = []

func construct(being_birth_dict: Dictionary) -> void:
	assert(being_birth_dict != null && being_birth_dict != {})
	
	#region are constructed inside mistate
	var sex: Enums.Sex
	var race: BasicRace
	var klass: Klass
	var faction: Faction
	var being_gen_template: BeingGenTemplate 
	#endregion
	var result
	
	result = handle_key(Keys.HEALTH_MULTIP, being_birth_dict)
	if result: mextra_health_multiplier = result; result = null
		
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
		mname = being_birth_dict[Keys.NAME]
	elif race.mdefault_being_names.size() > 0:
		mname = race.mdefault_being_names.pick_random()
	else:
		mname = "nameless"
	
	if race is ControllableRace:
		klass = handle_key(Keys.KLASS, being_birth_dict, race.mklasses)
	
	faction = handle_key(Keys.FACTION, being_birth_dict, GameData.factions)
	
	if being_birth_dict.has(Keys.BEING_GEN_TEMPLATE):
		being_gen_template = handle_key(Keys.BEING_GEN_TEMPLATE, being_birth_dict, Global.being_gen_templates)
	
	if being_birth_dict.has(Keys.FOLLOWERS):
		for follower_template_id in being_birth_dict[Keys.FOLLOWERS]:
			mfollowers.append(Global.being_gen_templates[follower_template_id])
	elif being_gen_template and being_gen_template.mav_followers_weighted_dist != null and not being_gen_template.mav_followers_weighted_dist.is_empty():
		if being_gen_template.mav_raid_points_for_followers == -1: # ignore raid point costs
			for follower_i in being_gen_template.max_followers_count:
				mfollowers.append(Global.being_gen_templates[WeightedChoice.pick(being_gen_template.mav_followers_weighted_dist)])
		elif being_gen_template.mav_raid_points_for_followers > 0:
			var remaining_points: int = being_gen_template.mav_raid_points_for_followers
			var cheapest_follower_points: int = being_gen_template.mav_followers_weighted_dist.values().min()
			
			while remaining_points > 0 and remaining_points >= cheapest_follower_points:
				var pick: BeingGenTemplate = Global.being_gen_templates[WeightedChoice.pick(being_gen_template.mav_followers_weighted_dist)]
				if pick.munit_raid_points <= remaining_points: 
					remaining_points -= pick.munit_raid_points
					mfollowers.append(pick)
		
	elif not being_gen_template and race is ControllableRace and klass.mselectable_followers != null and klass.mselectable_followers.size() > 0:
		mfollowers.append(klass.mselectable_followers.pick_random())
		
	if race.mhead_sprites_datas and race.mhead_sprites_datas.size() > 0:
		msprite_head = handle_key(Keys.HEAD, being_birth_dict, race.mhead_sprites_datas)
			
	msprite_body = handle_key(Keys.BODY, being_birth_dict, race.mbody_sprites_datas) as BodySpriteData
	
	result = handle_key(Keys.HEAD_SCALE, being_birth_dict)
	if result: mhead_scale = result; result = null
	
	result = handle_key(Keys.BODY_SCALE, being_birth_dict)
	if result: mbody_scale = result; result = null
	
	var sex_value = being_birth_dict[Keys.SEX]
	
	if sex_value is float and sex_value >= 0.0 and sex_value <= 1.0:
		var sex_probs: Dictionary = {Enums.Sex.MALE: sex_value, Enums.Sex.FEMALE: 1 - sex_value}
		sex = WeightedChoice.pick(sex_probs)
	elif sex_value is StringName or sex_value == Enums.Sex.ANY or (sex_value is float and (sex_value<0.0 or sex_value>1.0)):
		var sex_probs: Dictionary = {Enums.Sex.MALE: race.mmales_ratio, Enums.Sex.FEMALE: 1 - race.mmales_ratio}
		sex = WeightedChoice.pick(sex_probs)
	elif sex_value is Enums.Sex:
		sex = sex_value
	else:
		assert(false, "invalid type for \"sex\" entry in birth dict")
		
	

	assert(sex && race && faction)
	mistate = BeingInternalState.new()
	mistate.construct_for_posterior_serialization(sex, race, faction, null, klass, being_gen_template)

# TODO
func construct_from_serialized(serialized_being_spawn_data: Dictionary) -> void:
	pass
	
func serialize_being_internal_state() -> Dictionary:
	return mistate.serialize()

func serialize() -> Dictionary:
	var dict: Dictionary = {
		Keys.NAME: mname,
		Keys.HEAD_SCALE: mhead_scale, Keys.BODY_SCALE: mbody_scale,
		Keys.HEAD: msprite_head.mid, Keys.BODY: msprite_body.mid,
		Keys.INTERNAL_STATE: mistate.serialize(),
		Keys.HEALTH_MULTIP: mextra_health_multiplier,
		#extra_stats_multiplier,
	}
	dict[Keys.FOLLOWERS] = get_array_of_ids(mfollowers)
	return dict

#NO IMPLEMENTAR ESTA FUNCIÓN, PERO IMPLEMENTAR LA IDEA DE CARGAR STARTER CHARACTERS ASÍ NO PERDÉS TIEMPO RE-CREÁNDOLOS EN CADA LOBBY
#func _construct_from_saved_starter_character(starter_character: Resource) -> void: pass

# LEAVE WITHOUT TYPING FOR Array, OTHERWISE IT ISN'T SENDABLE THROUGH RPC
func get_array_of_ids(array_of_objects: Array) ->  Array:
	var array_ids: Array = []
	for o in array_of_objects:
		array_ids.push_back(o.mid)
	return array_ids

func handle_key(key: StringName, being_birth_dict: Dictionary, data_structure = null):
	if being_birth_dict.has(key):
		if data_structure != null and data_structure.is_empty():
			assert(false, "Global dict/array for key=%s is empty"%[key])
		if data_structure is Dictionary and data_structure.keys().size() > 0:
			if being_birth_dict[key] != &"random" && being_birth_dict[key] != "":
				return data_structure[being_birth_dict[key]]	
			else:
				return data_structure.values().pick_random()
		elif data_structure is Array and data_structure.size() > 0:
			for item in data_structure:
				if item == null:
					assert(false, "iterated data_structure has null (key=%s)"%[key])
				if being_birth_dict[key] == item.mid:
					return item
			return (data_structure as Array).pick_random()
		else: 
			return being_birth_dict[key]
	elif key != Keys.BODY_SCALE and key != Keys.HEAD_SCALE:
		push_warning("key %s not found" % key)
