extends Resource
class_name LandMassGenerator

@export var shape_generator: Script 
@export var tile_picker: Script

func generate(tile_map: TileMap, center: Vector2i, size: Vector2i, seed: int = 0) -> void:
	shape_generator.generate(tile_map, tile_picker, center, size, {},seed)
	
	
