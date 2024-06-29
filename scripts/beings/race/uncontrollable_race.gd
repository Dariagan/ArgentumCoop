extends BasicRace
class_name UncontrollableRace

@export_range(0, 1) var taming_difficulty: float = 0.5

#usar un solo script, tanto para el player behavior como para el stranger behavior. despues diferenciar con un if adentro 


#@export var behavior: NcbBehavior

#hacer tupla triple de id-item, probabilidad, cantidad. 
@export var possible_loot: Dictionary
