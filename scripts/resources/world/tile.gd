extends Resource
class_name Tile

@export var id: StringName

@export var layer: int = 0
@export var source_id: int
@export var atlas_pos: Vector2i = Vector2.ZERO
@export var alt_id: int

func get_id() -> StringName: return id

func get_layer() -> int: return layer

func get_source_id() -> int: return source_id

func get_atlas_pos() -> Vector2i: return atlas_pos

func get_alt_id() -> int: return alt_id
