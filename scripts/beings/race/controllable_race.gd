extends BasicRace
class_name ControllableRace

@export var mcombat_multipliers: CombatMultipliers = CombatMultipliers.new()
@export var mhead_sprites_datas: Array[SpriteData]

@export var mbody_sprites_datas: Array[BodySpriteData]
@export var mother_sprites_datas: Array[SpriteData]

@export var mklasses: Array[Klass]

@export var mmax_width_frontally_sideways_height: Vector3 = Vector3(1, 1, 1.3)
@export var mfallback_possible_loot: Dictionary


@export_category("Work Multipliers")

@export_range(0, 3) var mglobal_learning_multiplier: float = 1
@export_range(0, 3) var msmithing_learning_multiplier: float = 1
@export_range(0, 3) var mmanual_labor_multiplier: float = 1
@export_range(0, 3) var mresearch_multiplier: float = 1
@export_range(0, 3) var mtrade_proficiency: float = 1

func validate() -> bool:
	if mklasses.is_empty(): return false
	for klass: Klass in mklasses:
		if klass == null: return false
	return true
