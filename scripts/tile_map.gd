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
	
	
	
	generate_continent(Vector2i.ZERO, Vector2i(1500, 1500), seed)
	
	

const WATER_PROPORTION: float = -0.16

func generate_continent(center: Vector2i, size: Vector2i, seed: int = 0) -> void:
	seed=35
	
	var continent_alt: FastNoiseLite = FastNoiseLite.new()
	var peninsuler: FastNoiseLite = FastNoiseLite.new()
	
	var laker: FastNoiseLite = FastNoiseLite.new()

	continent_alt.seed = seed
	peninsuler.seed = seed
	laker.seed = seed
	
	continent_alt.noise_type = FastNoiseLite.TYPE_SIMPLEX
	continent_alt.frequency = 0.37/size.length()
	
	continent_alt.fractal_lacunarity = 2.8
	continent_alt.fractal_gain = 0.5
	continent_alt.fractal_weighted_strength = 0.6

	peninsuler.frequency = 6.4/size.length()
	peninsuler.fractal_gain = 0.56
	
	var lake_inlandiness_threshold: float = 0.7
	
	#peninsuler.fractal_lacunarity = 2
	
	
	#while peninsuler.get_noise_2dv(size/2) < WATER_PROPORTION + 0.1:
	#	peninsuler.offset.x += 10
	
	var continental_threshold: float = 0.57 * pow(size.length()/1600, 0.05)
	print(continental_threshold)
	
	while continent_alt.get_noise_2dv(center) < continental_threshold + 0.2:
		continent_alt.offset.x += 3
	
	var border_water_booster: Vector2 = Vector2.ZERO
	
	for i in range(-size.x/2, size.x/2):
		
		var bcf: float = calculate_border_closeness_factor(i, size.x)
		if bcf > 0.7: 
			border_water_booster.x = bcf
		else: border_water_booster.x = 0
		
		#mejor hacerlo radial y usar un easing
		
		for j in range(-size.y/2, size.y/2):
			var alt: float = peninsuler.get_noise_2d(i, j)
			var atlas_id: int
			var atlas_tile: Vector2i
			
			bcf = calculate_border_closeness_factor(j, size.y)
			if bcf > 0.7: 
				border_water_booster.y = bcf
			else: border_water_booster.y = 0
			
			if continent_alt.get_noise_2d(i,j) - border_water_booster.length_squared()/20 > continental_threshold and peninsuler.get_noise_2d(i,j) > WATER_PROPORTION:
				
				atlas_id = 0
				atlas_tile = Vector2i(0,1)
			else:
				atlas_id = 1
				atlas_tile = Vector2i.ZERO
				
				
			set_cell(0, Vector2i(center.x + i, center.y + j), atlas_id, atlas_tile)
	set_cell(0, Vector2i.ZERO, 1 , Vector2i(0, 9))
	

func calculate_border_closeness_factor(index_value: int, size: int) -> float:
	return abs(index_value)/(size/float(2))
