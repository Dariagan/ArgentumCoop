extends Resource
class_name Tile

@export var id: StringName

@export var layer: int = 0
@export var source_id: int
@export var tile_variants_atlas_origin_position: Array[Vector2i] = [Vector2i.ZERO]
@export var modulo_tile_picking_area: Vector2i = Vector2i(1, 1)# 4,4 for grass tiles
@export var alt_id: int


func get_data() -> Dictionary:
	return {
		"id" = id, 
		"layer" = layer, 
		"source_id" = source_id,
		"atlas_positions" = tile_variants_atlas_origin_position,
		"ma" = modulo_tile_picking_area,
		"alt_id" = alt_id
		}

