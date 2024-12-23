extends RustBeingGenTemplate
## akin to PawnKind in Rimworld
class_name BeingGenTemplate

@export var mrace: BasicRace
@export var mklass_id: StringName = &"random"
@export var mname: String = "BeingGenTemplate_missingname"

@export var mai_process: GDScript
@export var mdisplay_being_name: bool = false

@export var mbodies_distribution: Dictionary[SpriteData, float]
@export var mheads_distribution: Dictionary[BodySpriteData, float]

@export var mav_followers_weighted_dist: Dictionary[StringName, float] #key: follower BeingGenTemplate mid
@export var max_followers_count: int = 1
@export var mav_raid_points_for_followers: int = -1 # put as 0 to disable any followers from spawning even if mav_followers_weighted_dist has entries. -1 to ignore this and only take into account max_followers count

#if unset it uses the race's ratio
@export var mmales_ratio: float = -1

@export var mextra_health_multiplier_range: Vector2 = Vector2.ONE
@export var mhead_scale_range: Vector2 = Vector2.ONE
@export var mbody_scale_range: Vector2 = Vector2.ONE

#@export var names_distribution: Dictionary = {"placeholder_name": 1}
@export var mequipment_distribution: Dictionary[StringName, float]
@export var mloot_distribution: Dictionary[StringName, float]
@export var munit_raid_points: int = 100
@export var mdropped_xp_range: Vector2 = Vector2(10, 10)
@export var mcombat_multipliers: CombatMultipliers = CombatMultipliers.new()

func validate() -> bool: 
	var sum_of_weights: float = 0
	for follower_mid: StringName in mav_followers_weighted_dist.keys():
		var weight: float = mav_followers_weighted_dist[follower_mid]
		sum_of_weights += weight
		if weight < 0: return false
		if not Global.being_gen_templates[follower_mid] .mrace is UncontrollableRace: return false
	
	if not mav_followers_weighted_dist.keys().is_empty() and (sum_of_weights <= 0):
		push_error("sum of weights for available followers in klass %s is zero"%[mid])
		return false
	
	return mrace and Global.races.values().has(mrace)

func _instantiate_being_birth_dict() -> Dictionary:
	assert(self.mid and mrace)

	if mklass_id != &"random":
		assert(Global.klasses.has(mklass_id))

	assert(mextra_health_multiplier_range.x <= mextra_health_multiplier_range.y)
	assert(mhead_scale_range.x <= mhead_scale_range.y)
	assert(mbody_scale_range.x <= mbody_scale_range.y)
	assert(mdropped_xp_range.x <= mdropped_xp_range.y)
	for head: SpriteData in mheads_distribution:
		assert(mrace.head_sprites_datas.has(head))
	for body: BodySpriteData in mbodies_distribution:
		assert(mrace.head_sprites_datas.has(body))
	
	var h_scale: float = randf_range(mhead_scale_range.x, mhead_scale_range.y)
	var b_scale: float = randf_range(mbody_scale_range.x, mbody_scale_range.y)
	
	var being_birth_dict: Dictionary = {
		Keys.NAME: &"random",
		Keys.HEALTH_MULTIP: randf_range(mextra_health_multiplier_range.x, mextra_health_multiplier_range.y),
		Keys.RACE: mrace.mid,
		Keys.SEX: mmales_ratio,
		Keys.KLASS: mklass_id,
		Keys.HEAD: &"random" if not mheads_distribution else WeightedChoice.pick(mheads_distribution),
		Keys.BODY: &"random" if not mbodies_distribution else WeightedChoice.pick(mbodies_distribution),
		Keys.HEAD_SCALE: Vector3(h_scale, h_scale, h_scale),
		Keys.BODY_SCALE: Vector3(b_scale, b_scale, b_scale),
		#Keys.EQUIPMENT: null,
		Keys.BEING_GEN_TEMPLATE: self.mid, 
	}
	return being_birth_dict

func instantiate(faction: StringName) -> BeingStatePreIniter:
	var being_pre_init = BeingStatePreIniter.new()
	var birth_dict: Dictionary = _instantiate_being_birth_dict();
	birth_dict[Keys.FACTION] = faction
	being_pre_init.construct(birth_dict)
	return being_pre_init;
