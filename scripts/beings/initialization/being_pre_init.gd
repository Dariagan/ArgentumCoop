extends RefCounted 
#BeingRequiredInitializationData
class_name BeingPreInit
#se diferencia de un being_gen_template en q el being_gen_template puede incluir rangos de aleatoriedad/sets weighteados configurables para ciertos atributos/variables
#dentro de ahí, (money, possible loot, health, possible names). y en being_gen_template no se especifica la preiniter.mfaction. en esto sí, para saber en cual meter al being
#
# esto DEVUELVE solo valores absolutamente especificos y deterministas (pos-selección aleatoria de las distribuciones especificadas) para el
# spawning de un individuo específico con caracteristicas especificadas
# (excepto por la randomización de la cara y cuerpo si no se especifican)

var mname: String 
var mhead_scale: Vector3 = Vector3.ONE; var mbody_scale: Vector3 = Vector3.ONE
var msprite_head: SpriteData; var msprite_body: BodySpriteData
var mchosen_extra_sprites: Array[int] = []
var mextra_health_multiplier: float = 1
var msex: Enu.Sex
var mrace: BasicRace
var mklass: Klass
var mfaction: Faction
var mbeing_gen_templ: BeingGenTemplate
var mlevel: int

var mistate: BeingInternalState; var mfollowers: Array[BeingGenTemplate] = []

static func new_from_being_gen_templ(being_gen_template: BeingGenTemplate, faction: StringName) -> BeingPreInit:
	var preiniter = BeingPreInit.new()
	preiniter.mbeing_gen_templ = being_gen_template
	if being_gen_template.mav_followers_weighted_dist != null and not being_gen_template.mav_followers_weighted_dist.is_empty():
		if being_gen_template.mav_raid_points_for_followers == -1: # ignore raid point costs
			for follower_i in being_gen_template.max_followers_count:
				preiniter.mfollowers.append(Global.being_gen_templates[WeightedChoice.pick(being_gen_template.mav_followers_weighted_dist)])
		elif being_gen_template.mav_raid_points_for_followers > 0:
			var remaining_points: int = being_gen_template.mav_raid_points_for_followers
			var cheapest_follower_points: int = being_gen_template.mav_followers_weighted_dist.values().min()
			while remaining_points > 0 and remaining_points >= cheapest_follower_points:
				var pick: BeingGenTemplate = Global.being_gen_templates[WeightedChoice.pick(being_gen_template.mav_followers_weighted_dist)]
				if pick.munit_raid_points <= remaining_points: 
					remaining_points -= pick.munit_raid_points
					preiniter.mfollowers.append(pick)
	
	if being_gen_template.mmales_ratio != -1:
		if randf() < being_gen_template.mmales_ratio: preiniter.msex = Enu.Sex.MALE
		else: preiniter.msex = Enu.Sex.FEMALE
	else:
		if randf() < being_gen_template.mrace.mmales_ratio: preiniter.msex = Enu.Sex.MALE
		else: preiniter.msex = Enu.Sex.FEMALE
		
	if not being_gen_template.mheads_distribution:
		if being_gen_template.mrace.mhead_sprites_datas and not being_gen_template.mrace.mhead_sprites_datas.is_empty():
			preiniter.msprite_head = being_gen_template.mrace.mhead_sprites_datas.pick_random()
	else:
		preiniter.msprite_head = WeightedChoice.pick(being_gen_template.mheads_distribution)
	
	if not being_gen_template.mbodies_distribution:
		preiniter.msprite_body = being_gen_template.mrace.mbody_sprites_datas.pick_random()
	else:
		preiniter.msprite_body = WeightedChoice.pick(being_gen_template.mbodies_distribution)
		
	preiniter.mrace = being_gen_template.mrace
	
	if being_gen_template.mrace is ControllableRace:
		if being_gen_template.mklass_id == &"random":
			preiniter.mklass = being_gen_template.mrace.mklasses.pick_random()
		else:
			preiniter.mklass =  Global.klasses[being_gen_template.mklass_id]
	
	preiniter.mfaction = GameData.factions[faction]
	
	return preiniter


static func construct(being_birth_dict: Dictionary) -> BeingPreInit:
	assert(being_birth_dict != null && being_birth_dict != {})
	var preiniter = BeingPreInit.new()
	
	var result
	
	result = handle_key(Keys.HEALTH_MULTIP, being_birth_dict)
	if result: preiniter.mextra_health_multiplier = result; result = null
		
	#result = handle_key("level", being_birth_dict)
	#if result: level = result; result = null
		
	var race_id: StringName = being_birth_dict[Keys.RACE]
	
	if race_id == &"controllable_random":
		being_birth_dict[Keys.RACE] = &"random"
		preiniter.mrace = handle_key(Keys.RACE, being_birth_dict, Global.controllable_races)
	elif race_id == &"uncontrollable_random":
		being_birth_dict[Keys.RACE] = &"random"
		preiniter.mrace = handle_key(Keys.RACE, being_birth_dict, Global.uncontrollable_races)	
	elif Global.races[race_id] is ControllableRace:
		preiniter.mrace = handle_key(Keys.RACE, being_birth_dict, Global.controllable_races)
	elif Global.races[race_id] is UncontrollableRace:
		preiniter.mrace = handle_key(Keys.RACE, being_birth_dict, Global.uncontrollable_races)
	else:
		push_error("not a valid preiniter.mrace id")
		
	if being_birth_dict[Keys.NAME] != &"random":
		preiniter.mname = being_birth_dict[Keys.NAME]
	elif preiniter.mrace.mdefault_being_names.size() > 0:
		preiniter.mname = preiniter.mrace.mdefault_being_names.pick_random()
	else:
		preiniter.mname = "nameless"
	
	if preiniter.mrace is ControllableRace:
		preiniter.mklass = handle_key(Keys.KLASS, being_birth_dict, preiniter.mrace.mklasses)
	
	preiniter.mfaction = handle_key(Keys.FACTION, being_birth_dict, GameData.factions)
	
	assert(not being_birth_dict.has(Keys.BEING_GEN_TEMPLATE))
	
	if being_birth_dict.has(Keys.FOLLOWERS):
		for follower_template_id in being_birth_dict[Keys.FOLLOWERS]:
			preiniter.mfollowers.append(Global.being_gen_templates[follower_template_id])
		
	elif preiniter.mrace is ControllableRace and preiniter.mklass.mselectable_followers != null and preiniter.mklass.mselectable_followers.size() > 0:
		preiniter.mfollowers.append(preiniter.mklass.mselectable_followers.pick_random())
		
	if preiniter.mrace.mhead_sprites_datas and preiniter.mrace.mhead_sprites_datas.size() > 0:
		preiniter.msprite_head = handle_key(Keys.HEAD, being_birth_dict, preiniter.mrace.mhead_sprites_datas)
			
	preiniter.msprite_body = handle_key(Keys.BODY, being_birth_dict, preiniter.mrace.mbody_sprites_datas) as BodySpriteData
	
	result = handle_key(Keys.HEAD_SCALE, being_birth_dict)
	if result: preiniter.mhead_scale = result; result = null
	
	result = handle_key(Keys.BODY_SCALE, being_birth_dict)
	if result: preiniter.mbody_scale = result; result = null
	
	var sex_value = being_birth_dict[Keys.SEX]
	
	if sex_value is float and sex_value >= 0.0 and sex_value <= 1.0:
		if randf() < sex_value: preiniter.msex = Enu.Sex.MALE
		else: preiniter.msex = Enu.Sex.FEMALE
	elif sex_value is StringName or sex_value == Enu.Sex.ANY or (sex_value is float and (sex_value<0.0 or sex_value>1.0)):
		if randf() < preiniter.mrace.mmales_ratio: preiniter.msex = Enu.Sex.MALE
		else: preiniter.msex = Enu.Sex.FEMALE
	elif sex_value is Enu.Sex:
		preiniter.sex = sex_value
	else:
		assert(false, "invalid type for \"sex\" entry in birth dict")
		
	assert(preiniter.mrace && preiniter.mfaction)
	
	return preiniter

func serialize() -> Dictionary:
	
	
	var dict: Dictionary = {
		Keys.NAME: mname,
		Keys.HEAD_SCALE: mhead_scale, Keys.BODY_SCALE: mbody_scale,
		Keys.BODY: msprite_body.mid,
		Keys.SEX: msex,
		Keys.RACE: mrace.mid,
		Keys.HARMABLE_BODY: null,
		Keys.FACTION: mfaction.minstance_id,
		Keys.CHOSEN_EXTRA_HEAD_SPRITES: get_array_of_ids(mchosen_extra_sprites),
		Keys.HEALTH_MULTIP: mextra_health_multiplier,
		Keys.BEING_LEVEL: mlevel,
		#extra_stats_multiplier,
	}
	dict[Keys.FOLLOWERS] = get_array_of_ids(mfollowers)
	if msprite_head: dict[Keys.HEAD] = msprite_head.mid
	if mbeing_gen_templ: dict[Keys.BEING_GEN_TEMPLATE] = mbeing_gen_templ.mid
	if mklass: dict[Keys.KLASS] = mklass.mid
	return dict

#NO IMPLEMENTAR ESTA FUNCIÓN, PERO IMPLEMENTAR LA IDEA DE CARGAR STARTER CHARACTERS ASÍ NO PERDÉS TIEMPO RE-CREÁNDOLOS EN CADA LOBBY
#func _construct_from_saved_starter_character(starter_character: Resource) -> void: pass

# LEAVE WITHOUT TYPING FOR Array, OTHERWISE IT ISN'T SENDABLE THROUGH RPC
func get_array_of_ids(array_of_objects: Array) ->  Array:
	var array_ids: Array = []
	for o in array_of_objects:
		array_ids.push_back(o.mid)
	return array_ids

static func handle_key(key: StringName, being_birth_dict: Dictionary, data_structure = null):
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
