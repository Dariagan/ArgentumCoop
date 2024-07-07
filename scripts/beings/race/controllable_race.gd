extends BasicRace
class_name ControllableRace

@export var combat_multipliers: CombatMultipliers = CombatMultipliers.new()
@export var head_sprites_datas: Array[SpriteData]

@export var body_sprites_datas: Array[BodySpriteData]
@export var other_sprites_datas: Array[SpriteData]

@export var klasses: Array[Klass]

@export var max_width_frontally_sideways_height: Vector3 = Vector3(1, 1, 1.3)
@export var fallback_possible_loot: Dictionary


@export_category("Work Multipliers")

@export_range(0, 3) var global_learning_multiplier: float = 1
@export_range(0, 3) var smithing_learning_multiplier: float = 1
@export_range(0, 3) var manual_labor_multiplier: float = 1
@export_range(0, 3) var research_multiplier: float = 1
@export_range(0, 3) var trade_proficiency: float = 1
