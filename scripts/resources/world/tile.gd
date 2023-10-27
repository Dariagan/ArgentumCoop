extends Resource
class_name Tile

@export var id: StringName

@export var layer: int = 0
@export var source_id: int
@export var atlas_positions: Array[Vector2i] = [Vector2i.ZERO]
@export var alt_id: int

func get_data() -> Dictionary:
	return {
		"id" = id, 
		"layer" = layer, 
		"source_id" = source_id,
		"atlas_positions" = atlas_positions,
		"alt_id" = alt_id
		}

