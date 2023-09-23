extends Resource
class_name Tile

@export var id: StringName

@export var layer: int = 0
@export var source_id: int
@export var atlas_pos: Vector2i = Vector2.ZERO
@export var alt_id: int

func get_data() -> Dictionary:
	return {
		"id" = id, 
		"layer" = layer, 
		"source_id" = source_id,
		"atlas_pos" = atlas_pos,
		"alt_id" = alt_id
		}

