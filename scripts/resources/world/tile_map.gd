extends TileMap
class_name ProceduralTilemap

#2000x2000: 56 secs
const MAP_SIZE: Vector2i = Vector2i(2000, 2000)
#var temperature_submultiplier: FastNoiseLite = FastNoiseLite.new()

# si no está en el diccionario, la tile usa modulation default (1,1,1,1)
var modulation: Dictionary = {"41_22" = Color(1,1,1,1)}
# usar la modulation del tileset si es una tile plana (sin noise)
	

var world: Array[Array] = get_world_matrix(MAP_SIZE)

func get_world_matrix(dimensions: Vector2i, append_array: bool = true) -> Array[Array]:
	var matrix: Array[Array] = []
	for i in range(dimensions.x):
		matrix.append([])
		for j in range(dimensions.y):
			if append_array: matrix[i].append([])
			else: matrix[i].append(false)
	return matrix

#hacer todo lo de generate_world un script desacoplado, así pueden haber muchas generaciones de mundo posibles y haces mundos raros nase
var world_generator: Script
@rpc("call_local")
func generate_world(seed: int = 0, data: Dictionary = {}) -> void:
	
	var empty_tiles: Array[Vector2i] = []#meter ocean
	
	var dcg: LandMassGenerator = preload("res://resources/world/default_continent_generator.tres")
	
	dcg.generate(world, Vector2i.ZERO, Vector2i(2000,2000), 0)


#hacer esto en C#?
var loaded_tiles: Array[Array] = get_world_matrix(MAP_SIZE, false)
var loaded_tiles_count: int = 0

func load_tiles_for_being(being_pos: Vector2) -> void:
	var being_coords: Vector2i = local_to_map(being_pos)
	
	var chunk_size: Vector2 = Vector2(90, 53)
	#var chunk_size: Vector2 = Vector2(70, 50) / being.camera_2d.zoom
	#chunk_size = clamp(chunk_size, Vector2(120, 70), Vector2(300, 220))
	
	#hacer q solo cargue chunks en direccion a donde apuntas?
	for i in range(-chunk_size.x/2, chunk_size.x/2):
		for j in range(-chunk_size.y/2, chunk_size.y/2):
			var matrix_coords: Vector2i = Vector2i(MAP_SIZE.x/2 + being_coords.x + i, MAP_SIZE.y/2 + being_coords.y + j)
			if matrix_coords.x < (MAP_SIZE.x - 1) and matrix_coords.y < (MAP_SIZE.y - 1) and matrix_coords.x >= 0 and matrix_coords.y >= 0:
				for tile in world[matrix_coords.x][matrix_coords.y] as Array[Tile]:
					var tilemap_tile_coords: Vector2i = Vector2i(being_coords.x + i, being_coords.y + j)
					
					if not loaded_tiles[matrix_coords.x][matrix_coords.y]:
						set_cell(tile.layer, tilemap_tile_coords, tile.source_id, tile.atlas_pos, tile.alternative_id)
						loaded_tiles[matrix_coords.x][matrix_coords.y] = tilemap_tile_coords
						loaded_tiles_count += 1
			else:
				#poner otra cosa, como fog, así no queda todo gris
				pass
	
	unload_excess_tiles(being_coords, loaded_tiles)
	
var ais: Array[Being]
func unload_excess_tiles(being_coords: Vector2i, loaded_tiles: Array[Array]) -> void:
	
	const MAX_LOADED_TILES: int = 30000
	
	if loaded_tiles_count > MAX_LOADED_TILES:
		for i in range(loaded_tiles.size()):
			for j in range(loaded_tiles[0].size()):
				if loaded_tiles[i][j] and Vector2(being_coords).distance_squared_to(loaded_tiles[i][j]) > 27000:
					for layer_i in range(get_layers_count()):
						erase_cell(layer_i, loaded_tiles[i][j])
					loaded_tiles[i][j] = false
					loaded_tiles_count -= 1


	
	
