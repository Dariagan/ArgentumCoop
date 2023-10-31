extends Resource
class_name TileSelection

@export var id: StringName

@export var tiles: Dictionary
#key: lake/beach/continental/tree. value: id del tile/id definida abajo del grupo de tiles

@export var grouped_prob_weighted_tiles: Dictionary
# key = id definida del grupo de tiles agrupadas. value = Array[(tile_id, assigned_weight)]
