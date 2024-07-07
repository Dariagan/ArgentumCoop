extends BasicRace
class_name UncontrollableRace

@export_range(0, 1) var taming_difficulty: float = 0.5
@export var combat_multipliers: CombatMultipliers = CombatMultipliers.new()
@export var head_sprites_datas: Array[SpriteData]

@export var body_sprites_datas: Array[BodySpriteData]

#usar un solo script, tanto para el player behavior como para el stranger behavior. despues diferenciar con un if adentro 


#@export var behavior: NcbBehavior

#hacer tupla triple de id-item, probabilidad, cantidad. 
@export var fallback_possible_loot: Dictionary
