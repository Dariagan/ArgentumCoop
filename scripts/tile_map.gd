extends TileMap
class_name ProceduralTilemap

#2000x2000: 56 secs
const MAP_SIZE: Vector2i = Vector2i(6000, 6000)
@export var test: FastNoiseLite
@export var test2: FastNoiseLite
#var temperature_submultiplier: FastNoiseLite = FastNoiseLite.new()




# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.




func generate_world(seed: int = 0) -> void:#randi()
	
	var empty_tiles: Array[Vector2i] = []
	
	
	
	generate_continent(Vector2i.ZERO, Vector2i(2000, 2000), seed)
	
	

const peninsuler_cutoff: float = -0.1

func generate_continent(center: Vector2i, size: Vector2i, seed: int = 0) -> void:
	seed=200
	
	var continent_alt: FastNoiseLite = FastNoiseLite.new()
	var peninsuler: FastNoiseLite = FastNoiseLite.new()
	
	var laker: FastNoiseLite = FastNoiseLite.new()

	continent_alt.seed = seed
	peninsuler.seed = seed
	laker.seed = seed
	
	
	continent_alt.noise_type = FastNoiseLite.TYPE_SIMPLEX
	continent_alt.frequency = 0.4/size.length()
	
	continent_alt.fractal_lacunarity = 2.8
	continent_alt.fractal_gain = 0.5
	continent_alt.fractal_weighted_strength = 0.5

	peninsuler.noise_type = FastNoiseLite.TYPE_SIMPLEX
	peninsuler.frequency = 5/size.length()
	peninsuler.fractal_gain = 0.56
	
	var continental_threshold: float = 0.6 * pow(size.length()/1600, 0.05)
	print(continental_threshold)
	
	laker.frequency = 10/size.length()

	var lake_cutoff: float = -0.4

	
	while continent_alt.get_noise_2dv(center) < continental_threshold + 0.04:
		continent_alt.offset.x += 3
	
	for i in range(-size.x/2, size.x/2):
		
		for j in range(-size.y/2, size.y/2):
			var alt: float = peninsuler.get_noise_2d(i, j)
			var atlas_id: int
			var atlas_tile: Vector2i
			
			# border closeness factor
			var bcf = (Vector2(i,j).distance_to(center))/(size.length()/float(2))
			
			var landmassness: float = continent_alt.get_noise_2d(i,j) - pow(bcf, 4)
			
			var away_from_coast: bool = landmassness > continental_threshold + 0.06 and peninsuler.get_noise_2d(i,j) > peninsuler_cutoff + 0.27
			
			#volver match statement?
			if landmassness > continental_threshold and peninsuler.get_noise_2d(i,j) > peninsuler_cutoff:
				
				if away_from_coast and laker.get_noise_2d(i,j) > lake_cutoff:
					atlas_id = 1
					atlas_tile = Vector2i(0, 9)
				else:
					atlas_id = 0
					atlas_tile = Vector2i(0,1)
				
			else:
				atlas_id = 1
				atlas_tile = Vector2i.ZERO
				
				
			set_cell(0, Vector2i(center.x + i, center.y + j), atlas_id, atlas_tile)
	set_cell(0, Vector2i.ZERO, 1 , Vector2i(0, 9))
	

func calculate_border_closeness_factor(index_value: int, size: int) -> float:
	return abs(index_value)/(size/float(2))
