extends BasicRace
class_name UncontrollableRace

@export_range(0, 1) var taming_difficulty: float = 0.5

#@export var behavior: NcbBehavior

#hacer tupla triple de id-item, probabilidad, cantidad. 
@export var possible_loot: Dictionary
