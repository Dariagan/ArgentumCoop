extends Resource
class_name TileSelection

@export var id: StringName
@export var targets: Array[String] 
@export var tile_to_place: Array[String] # empezar con _ (underscore) si es un grupo de tiles con probs

#TODO SUBSTRINGEAR CADA TILE DEL GROUP
@export var grouped_prob_weighted_tiles: Array[Dictionary]
#ES MUCHO MÁS FÁCIL EDITAR EL RECURSO DESDE UN TEXT EDITOR SOBRE EL .TRES
