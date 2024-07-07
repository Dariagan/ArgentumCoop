extends RustBeingGenTemplate
## akin to PawnKind in Rimworld
class_name BeingGenTemplate

@export var race_id: StringName
@export var klass_id: StringName = &"random"


@export var ai_process: GDScript

@export var display_being_name: bool = false

#meter los spritesdatas directamente?
@export var bodies_distribution: Dictionary
@export var heads_distribution: Dictionary

#if unset it uses the race's ratio
@export var males_ratio: float = -1

@export var extra_health_multiplier_range: Vector2 = Vector2.ONE
@export var head_scale_range: Vector2 = Vector2.ONE
@export var body_scale_range: Vector2 = Vector2.ONE

#@export var names_distribution: Dictionary = {"placeholder_name": 1}
@export var equipment_distribution: Dictionary
@export var loot_distribution: Dictionary
@export var raid_unit_cost: int = 100
@export var dropped_xp_range: Vector2 = Vector2(10, 10)
@export var combat_multipliers: CombatMultipliers = CombatMultipliers.new()


func _instantiate_being_birth_dict() -> Dictionary:
	assert(self.id and race_id)
	if Global.controllable_races.has(race_id):
		if klass_id != &"random":
			assert(Global.klasses.has(klass_id))
	else:
		assert(Global.uncontrollable_races.has(race_id))
	
	assert(extra_health_multiplier_range.x <= extra_health_multiplier_range.y)
	assert(head_scale_range.x <= head_scale_range.y)
	assert(body_scale_range.x <= body_scale_range.y)
	assert(dropped_xp_range.x <= dropped_xp_range.y)
	var race: BasicRace = Global.races[race_id]
	for head: SpriteData in heads_distribution:
		assert(race.head_sprites_datas.has(head))
	for body: BodySpriteData in bodies_distribution:
		assert(race.head_sprites_datas.has(body))
	
	var h_scale: float = randf_range(head_scale_range.x, head_scale_range.y)
	var b_scale: float = randf_range(body_scale_range.x, body_scale_range.y)
	
	var being_birth_dict: Dictionary = {
		Keys.NAME: &"random",
		Keys.HEALTH_MULTIP: randf_range(extra_health_multiplier_range.x, extra_health_multiplier_range.y),
		Keys.RACE: race_id,
		Keys.SEX: males_ratio,
		Keys.KLASS: klass_id,
		Keys.HEAD: &"random" if not heads_distribution else WeightedChoice.pick(heads_distribution),
		Keys.BODY: &"random" if not bodies_distribution else WeightedChoice.pick(bodies_distribution),
		Keys.HEAD_SCALE: Vector3(h_scale, h_scale, h_scale),
		Keys.BODY_SCALE: Vector3(b_scale, b_scale, b_scale),
		#Keys.EQUIPMENT: null,
		Keys.BEING_GEN_TEMPLATE: id, 
	}
	return being_birth_dict

func instantiate(faction: StringName) -> BeingStatePreIniter:
	var being_pre_init = BeingStatePreIniter.new()
	var birth_dict: Dictionary = _instantiate_being_birth_dict();
	birth_dict[Keys.FACTION] = faction
	being_pre_init.construct(birth_dict)
	return being_pre_init;
