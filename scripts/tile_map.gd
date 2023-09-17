extends TileMap
class_name ProceduralTilemap

#2000x2000: 56 secs
const MAP_SIZE: Vector2i = Vector2i(1500, 1500)
@export var test: FastNoiseLite
@export var test2: FastNoiseLite
#var temperature_submultiplier: FastNoiseLite = FastNoiseLite.new()

# si no está en el diccionario, la tile usa modulation default (1,1,1,1)
var modulation: Dictionary = {"41_22" = Color(1,1,1,1)}
# usar la modulation del tileset si es una tile plana (sin noise)
	
var world_generator: Script
#hacer todo lo de generate_world un script desacoplado, así pueden haber muchas generaciones de mundo posibles y haces mundos raros nase

var world: Array[Array] = get_world_matrix(MAP_SIZE)

func get_world_matrix(dimensions: Vector2i, append_array: bool = true) -> Array[Array]:
	var matrix: Array[Array] = []
	for i in range(dimensions.x):
		matrix.append([])
		if append_array:
			for j in range(dimensions.y):
				matrix[i].append([])
	return matrix

signal world_generated

@rpc("call_local")
func generate_world(seed: int = 0) -> void:#randi()
	
	var empty_tiles: Array[Vector2i] = []#meter ocean
	
	var lmg: LandMassGenerator = preload("res://resources/world/default_continent_generator.tres")
	
	lmg.generate(world, Vector2i.ZERO, Vector2i(1000, 1000), 0)
	
	world_generated.emit()
	
				
func spawn_active_being(being: Being) -> void:
	add_child(being)
	being.load_tiles_around_me.connect(load_tiles_for_being)




var clear_times: Dictionary = {}

#los npcs ejecutan esto si estan en un estado de persecución sobre el jugador y necesitan la data del terreno

var loaded_tiles: Dictionary = {}

func load_tiles_for_being(being: Being) -> void:
	var being_coords: Vector2i = local_to_map(being.position)
	
	var chunk_size: Vector2 = Vector2(150, 110)
	#var chunk_size: Vector2 = Vector2(70, 50) / being.camera_2d.zoom
	#chunk_size = clamp(chunk_size, Vector2(120, 70), Vector2(300, 220))
	
	for i in range(-chunk_size.x/2, chunk_size.x/2):
		for j in range(-chunk_size.y/2, chunk_size.y/2):
			for tile in world[MAP_SIZE.x/2 + being_coords.x + i][MAP_SIZE.y/2 + being_coords.y + j] as Array[Tile]:
				var tile_coords: Vector2i = Vector2i(being_coords.x + i, being_coords.y + j)
				
				var string_tile_coords: String = "%d_%d" % [tile_coords.x, tile_coords.y]
				
				if not loaded_tiles.has(string_tile_coords):
					set_cell(tile.layer, tile_coords, tile.source_id, tile.atlas_pos, tile.alternative_id)
					loaded_tiles[string_tile_coords] = true
	
	#unload_excess_tiles(being_coords, loaded_tiles)
	
var active_ais: Array[Being]
func unload_excess_tiles(being_coords: Vector2i, loaded_tiles: Dictionary) -> void:
	
	const MAX_LOADED_TILES: int = 80000
	
	if loaded_tiles.keys().size() > MAX_LOADED_TILES:
		for loaded_tile in loaded_tiles.keys() as Array[String]:
			
			var split_floats: PackedFloat64Array = loaded_tile.split_floats("_", false) 
			var tile_position: Vector2i = Vector2i(split_floats[0], split_floats[1])
			
			if Vector2(being_coords).distance_squared_to(tile_position) > 45000:
				
				loaded_tiles.erase(loaded_tile)
				
				for layer_i in range(get_layers_count()):
					erase_cell(layer_i, tile_position)
		
	
	
	
