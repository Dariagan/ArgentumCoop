extends Resource
class_name Tile

# NO MORE THAN 31 CHARACTERS
@export var id: StringName
	
@export var layer: int = 0
@export var source_atlas: int = -1
@export var origin_position:Vector2i = Vector2i.ZERO
@export var flipped_at_random: bool = false

@export var random_scale_range: Vector4 # TODO tal vez es mejor hacer los bushes y trees escenas para poder hacer esto
#borrar la linea de arriba desp

@export var modulo_tiling_area: Vector2i = Vector2i(1, 1)
@export var alt_id: int

func get_data() -> Dictionary:
	
	assert(id != &"" and source_atlas != -1 and modulo_tiling_area.x>0 and modulo_tiling_area.y>0 and alt_id>=0)
		
	return {
		&"id" : id, 
		&"layer" : layer, 
		&"source_atlas" : source_atlas,
		&"fr" : flipped_at_random,
		&"op" : origin_position,
		&"ma" : modulo_tiling_area,
		&"alt_id" : alt_id
		}
