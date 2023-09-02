extends TileMap
class_name ProceduralTilemap

#2000x2000: 56 secs
const MAP_SIZE: Vector2i = Vector2i(1601, 1601)
@export var test: FastNoiseLite
@export var test2: FastNoiseLite
#var temperature_submultiplier: FastNoiseLite = FastNoiseLite.new()

# si no está en el diccionario, la tile usa modulation default (1,1,1,1)
var modulation: Dictionary = {"41_22" = Color(1,1,1,1)}
# usar la modulation del tileset si es una tile plana (sin noise)
	
var world_generator: Script
#hacer todo lo de generate_world un script desacoplado, así pueden haber muchas generaciones de mundo posibles y haces mundos raros nase

var world: Array[Array] = []

func fill_world_matrix(world: Array[Array], dimensions: Vector2i) -> void:
	for i in range(dimensions.x):
		world.append([])
		for j in range(dimensions.y):
			world[i].append([])

signal world_generated

@rpc("call_local")
func generate_world(seed: int = 0) -> void:#randi()
	
	var empty_tiles: Array[Vector2i] = []
	
	var asd: LandMassGenerator = preload("res://resources/world/default_continent_generator.tres")
	
	fill_world_matrix(world, MAP_SIZE)
	
	asd.generate(world, Vector2i.ZERO, Vector2i(1000, 1000), 0)
	
	world_generated.emit()
	
				
func spawn(being: Being) -> void:
	add_child(being)
	being.load_tiles_around_me.connect(load_tiles_for_being)
	await world_generated
	load_tiles_for_being(being)



var clear_times: Dictionary = {}

#los npcs ejecutan esto si estan en un estado de persecución sobre el jugador y necesitan la data del terreno

func load_tiles_for_being(being: Being) -> void:
	var coords: Vector2i = local_to_map(being.position)
	var chunk_size = Vector2(70, 50) / being.camera_2d.zoom
	
	chunk_size = clamp(chunk_size, Vector2(120, 70), Vector2(300, 220))
	

	clear()#hacer q solo haga esto cada 30 segundos.
	# si un npc ejecuta esto, te borra tu visión. arreglar.
		
	

	for i in range(-chunk_size.x/2, chunk_size.x/2):
		for j in range(-chunk_size.y/2, chunk_size.y/2):
			for tile in world[MAP_SIZE.x/2 + coords.x + i][MAP_SIZE.y/2 + coords.y + j] as Array[Tile]:
				set_cell(tile.layer, Vector2i(coords.x + i, coords.y + j), tile.source_id, tile.atlas_pos, tile.alternative_id)
				
		
	
	
	#para el resto de continentes, hay q poner areas de dimensiones el size de la generación + 10%, lo cual trigerrea su generación
	#(sino es mucho lag hacerlo todo de una)
	
