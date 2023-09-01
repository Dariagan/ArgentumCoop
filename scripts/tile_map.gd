extends TileMap
class_name ProceduralTilemap

#2000x2000: 56 secs
const MAP_SIZE: Vector2i = Vector2i(6000, 6000)
@export var test: FastNoiseLite
@export var test2: FastNoiseLite
#var temperature_submultiplier: FastNoiseLite = FastNoiseLite.new()

# si no está en el diccionario, la tile usa modulation default (1,1,1,1)
var modulation: Dictionary = {"41_22" = Color(1,1,1,1)}
# usar la modulation del tileset si es una tile plana (sin noise)
	
var world_generator: Script

#hacer todo es
@rpc("call_local")
func generate_world(seed: int = 0) -> void:#randi()
	
	var empty_tiles: Array[Vector2i] = []
	
	var asd: LandMassGenerator = preload("res://resources/world/default_continent_generator.tres")
	
	asd.generate(self, Vector2i.ZERO, Vector2i(1750, 1750), 0)
	
	#para el resto de continentes, hay q poner areas de dimensiones el size de la generación + 10%, lo cual trigerrea su generación
	#(sino es mucho lag hacerlo todo de una)
	
