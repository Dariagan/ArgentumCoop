extends BasicRace
class_name UncontrollableRace

@export_range(0, 1) var mtaming_difficulty: float = 0.5
@export var mcombat_multipliers: CombatMultipliers = CombatMultipliers.new()
@export var mhead_sprites_datas: Array[SpriteData]

@export var mbody_sprites_datas: Array[BodySpriteData]

#usar un solo script, tanto para el player behavior como para el stranger behavior. despues diferenciar con un if adentro 


#@export var behavior: NcbBehavior

#hacer tupla triple de id-item, probabilidad, cantidad. 
@export var mfallback_possible_loot: Dictionary
