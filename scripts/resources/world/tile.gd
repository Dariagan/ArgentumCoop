extends Resource
class_name Tile

@export var id: StringName

@export var layer: int = 0
@export var source_id: int
@export var origin_position:Vector2i = Vector2i.ZERO
#borrar la linea de arriba desp

@export var modulo_tiling_area: Vector2i = Vector2i(1, 1)
@export var alt_id: int



func get_data() -> Dictionary:
	return {
		"id" = id, 
		"layer" = layer, 
		"source_id" = source_id,
		"op" = origin_position,
		"ma" = modulo_tiling_area,
		"alt_id" = alt_id
		}

