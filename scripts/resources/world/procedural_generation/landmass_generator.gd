extends Resource
class_name LandMassGenerator

@export var shape_generator: Script 
@export var tile_picker: Script

#hacer que devuelva un array (o dict, lo q más convenga) de (coord, tile)? de esta forma, se lee de esta lista 
# y dinámicamente en el tilemap solo se generan las tiles q estén cerca tuyo (area cuadrada). y se despawnean las lejanas
#así no hay q hacer cosas raras de definir zonas de generación



func generate(world: Array[Array], center: Vector2i, size: Vector2i, seed: int = 0) -> void:
	
	assert (size.x <= world.size() and size.y <= world[0].size())  
	
	shape_generator.generate(world, tile_picker, center, size, {}, seed)
	
	
