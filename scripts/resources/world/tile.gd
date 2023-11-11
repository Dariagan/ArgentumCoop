extends Resource
class_name Tile

# NO MORE THAN 31 CHARACTERS
@export var id: StringName

@export var layer: int = 0
@export var source_id: int
@export var origin_position:Vector2i = Vector2i.ZERO
#borrar la linea de arriba desp

@export var modulo_tiling_area: Vector2i = Vector2i(1, 1)
@export var alt_id: int

func get_data() -> Dictionary:
	if id.length() > 31:
		var original_id: String = id
		id = id.substr(31 % id.length())
		printerr("using substring of tile_id '%s' -> '%s'" % [original_id, id])
		
	return {
		"id" = id, 
		"layer" = layer, 
		"source_id" = source_id,
		"op" = origin_position,
		"ma" = modulo_tiling_area,
		"alt_id" = alt_id
		}

