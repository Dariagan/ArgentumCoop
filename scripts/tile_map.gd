extends TileMap
class_name ProceduralTilemap

#2000x2000: 56 secs
const MAP_SIZE: Vector2i = Vector2i(6000, 6000)
@export var test: FastNoiseLite
#var temperature_submultiplier: FastNoiseLite = FastNoiseLite.new()


const WATER_PROPORTION: float = -0.4

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.




func generate_world(seed: int = 0) -> void:#randi()
	
	var empty_tiles: Array[Vector2i] = []
	
	
	
	generate_continent(Vector2i.ZERO, Vector2i(1500, 1500), seed)
	
	

func generate_continent(center: Vector2i, size: Vector2i, seed: int = 0) -> void:
	
	var altitude: FastNoiseLite = FastNoiseLite.new()
	var continent_shape: FastNoiseLite = FastNoiseLite.new()
	
	continent_shape.frequency = 0.0006
	continent_shape.seed = seed
	
	continent_shape.noise_type = FastNoiseLite.TYPE_SIMPLEX
	continent_shape.fractal_lacunarity = 2.9
	
	altitude.seed = seed
	altitude.frequency = 0.008
	#altitude.fractal_type = FastNoiseLite.FRACTAL_PING_PONG
	
	
	while continent_shape.get_noise_2dv(size/2) < 0.5:
		continent_shape.offset.x += 20
	while altitude.get_noise_2dv(size/2) < WATER_PROPORTION + 0.1:
		altitude.offset.x += 10
	
	for i in range(size.x):
		for j in range(size.x):
			var alt: float = altitude.get_noise_2d(i, j)
			var atlas_id: int
			var atlas_tile: Vector2i
			
			if continent_shape.get_noise_2d(i,j) > 0.2 and altitude.get_noise_2d(i,j) > WATER_PROPORTION:
				atlas_id = 0
				atlas_tile = Vector2i(0,1)
			else:
				atlas_id = 1
				atlas_tile = Vector2i.ZERO
			set_cell(0, Vector2i(center.x - size.x/2 + i, center.y - size.y/2 + j), atlas_id, atlas_tile)
	set_cell(0, Vector2i.ZERO, 1 , Vector2i(0, 9))
	
	
