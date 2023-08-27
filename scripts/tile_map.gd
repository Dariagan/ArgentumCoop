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



@rpc("call_local")
func generate_world(seed: int = 0) -> void:#randi()
	
	var empty_tiles: Array[Vector2i] = []

	generate_continent(Vector2i.ZERO, Vector2i(2100, 2100), seed)
	
	

const peninsuler_cutoff: float = -0.1

func generate_continent(center: Vector2i, size: Vector2i, seed) -> void:
	seed=8
	
	var continent_alt: FastNoiseLite = FastNoiseLite.new()
	var peninsuler: FastNoiseLite = FastNoiseLite.new()
	var laker: FastNoiseLite = FastNoiseLite.new()
	var beacher: FastNoiseLite = FastNoiseLite.new()
	
	var rng = RandomNumberGenerator.new()
	rng.seed = seed
	#usar para randomizar (dentro de un rango sensible) los números de configuración de la generación (generar entre 0,8 y 1,2)

	continent_alt.seed = seed
	peninsuler.seed = seed
	laker.seed = seed
	beacher.seed = seed
	
	
	continent_alt.noise_type = FastNoiseLite.TYPE_SIMPLEX
	continent_alt.frequency = 0.32/size.length()
	
	continent_alt.fractal_lacunarity = 2.8
	continent_alt.fractal_gain = 0.5
	continent_alt.fractal_weighted_strength = 0.5

	peninsuler.noise_type = FastNoiseLite.TYPE_SIMPLEX
	peninsuler.frequency = 5/size.length()
	peninsuler.fractal_gain = 0.56
	
	beacher.noise_type #= FastNoiseLite.TYPE_SIMPLEX
	beacher.frequency = 9/size.length()
	beacher.fractal_octaves = 1
	
	
	var continental_cutoff: float = 0.6 * pow(size.length()/1600, 0.05)
	print(continental_cutoff)
	
	laker.frequency = 26/size.length()
	laker.fractal_type = FastNoiseLite.FRACTAL_NONE

	const lake_cutoff: float = 0.83

	
	while continent_alt.get_noise_2dv(center) < continental_cutoff + 0.1:
		continent_alt.offset.x += 3
	
	for i in range(-size.x/2, size.x/2):
		for j in range(-size.y/2, size.y/2):
			var alt: float = peninsuler.get_noise_2d(i, j)
			var atlas_id: int
			var atlas_tile: Vector2i
			
			# border closeness factor
			var bcf = Vector2(i,j).distance_to(center)/(size.length()/float(2))
			var continentness: float = continent_alt.get_noise_2d(i,j) - pow(bcf, 4)
			
			var continental: bool = continentness > continental_cutoff
			var peninsuler_caved: bool = peninsuler.get_noise_2d(i,j) < peninsuler_cutoff
			
			var away_from_coast: bool = continentness > continental_cutoff + 0.06 and peninsuler.get_noise_2d(i,j) > peninsuler_cutoff + 0.27
			var lake_tile: bool = laker.get_noise_2d(i,j) > lake_cutoff
			
			var beach_cutoff: float = 0.86
			var coastness: float = maxf(0.6 - pow((continentness - continental_cutoff), 0.58), (0.95 - pow((peninsuler.get_noise_2d(i,j) - peninsuler_cutoff), 0.50)))
			
			var beach: bool = beacher.get_noise_2d(i,j)/1.7 + coastness > beach_cutoff
			var lake: bool = laker.get_noise_2d(i,j)/1.3 + 1 - pow(coastness, 0.67) > lake_cutoff
			
			
			match [continental, peninsuler_caved, away_from_coast, lake, beach]:
				
				[true, false, false, _, true]:#beach
					atlas_id = 2; atlas_tile = Vector2i(1,0)
					
				[true, false, true, true, ..]:#lake
					atlas_id = 1; atlas_tile = Vector2i(9, 0)
				#[true, true, ..]: atlas_id = 0;atlas_tile = Vector2i(10,1) #peninsulardebug
				[false, ..], [true, true, ..]:#ocean
					atlas_id = 1; atlas_tile = Vector2i.ZERO
					#TODO HACER ANIMATED WATER
				
				[true, false, ..]:#grass
					atlas_id = 0; atlas_tile = Vector2i(0,1)
			"""
			[true, true, ..]:#peninsulercaveinocean (debug)
				atlas_id = 0;atlas_tile = Vector2i(10,1)
			"""
				
			set_cell(0, Vector2i(center.x + i, center.y + j), atlas_id, atlas_tile)
	set_cell(0, Vector2i.ZERO, 1 , Vector2i(0, 9))
	

func calculate_border_closeness_factor(index_value: int, size: int) -> float:
	return abs(index_value)/(size/float(2))
