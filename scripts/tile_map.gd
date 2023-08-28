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

	#var size = 800
	generate_continent(Vector2i.ZERO, Vector2i(1300, 1300), seed)
	
	



func generate_continent(center: Vector2i, size: Vector2i, seed) -> void:
	seed=4343
	
	var continent_alt: FastNoiseLite = FastNoiseLite.new()
	var peninsuler: FastNoiseLite = FastNoiseLite.new()
	var laker: FastNoiseLite = FastNoiseLite.new()
	var beacher: FastNoiseLite = FastNoiseLite.new()
	var p_beacher: FastNoiseLite = FastNoiseLite.new()
	continent_alt.seed = seed
	peninsuler.seed = seed
	laker.seed = seed
	beacher.seed = seed
	p_beacher.seed = seed + 1
	
	var rng = RandomNumberGenerator.new()
	rng.seed = seed
	#usar para randomizar (dentro de un rango sensible) los números de configuración de la generación (generar entre 0,8 y 1,2)
	
	continent_alt.noise_type = FastNoiseLite.TYPE_SIMPLEX
	continent_alt.frequency = 0.14/size.length()
	
	continent_alt.fractal_lacunarity = 2.8
	continent_alt.fractal_weighted_strength = 0.5
	var continental_cutoff: float = 0.6 * pow(size.length()/1600, 0.05)

	peninsuler.noise_type = FastNoiseLite.TYPE_SIMPLEX
	peninsuler.frequency = 5/size.length()
	peninsuler.fractal_gain = 0.56
	const peninsuler_cutoff: float = -0.1
	
	beacher.noise_type = FastNoiseLite.TYPE_SIMPLEX_SMOOTH
	beacher.frequency = 5/size.length()
	const beach_cutoff: float = 0.8
	
	p_beacher.noise_type = FastNoiseLite.TYPE_SIMPLEX_SMOOTH
	p_beacher.frequency = 10/size.length()
	p_beacher.fractal_octaves = 1
	
	laker.frequency = 15/size.length()
	const lake_cutoff: float = 0.5
	
	var dungeon_coords: Array[Vector2i]
	
	while continent_alt.get_noise_2dv(center) < continental_cutoff + 0.13:
		continent_alt.offset.x += 3
		continent_alt.offset.y += 3
	
	for i in range(-size.x/2, size.x/2):
		for j in range(-size.y/2, size.y/2):
			var alt: float = peninsuler.get_noise_2d(i, j)
			
			var bcf: float = max(abs(i - center.x)/(size.x/float(2)), abs(j - center.y)/(size.y/float(2)),)
			
			var continentness: float = continent_alt.get_noise_2d(i,j) - pow(bcf, 43) - bcf/4.7
			
			var continental: bool = continentness > continental_cutoff
			var peninsuler_caved: bool = peninsuler.get_noise_2d(i,j) < peninsuler_cutoff
			
			var away_from_coast: bool = continentness > continental_cutoff + 0.06 and peninsuler.get_noise_2d(i,j) > peninsuler_cutoff + 0.27
			var lake_tile: bool = laker.get_noise_2d(i,j) > lake_cutoff
			
			var coastness: float = maxf(0.72 + beacher.get_noise_2d(i,j)/2.3 - pow((continentness - continental_cutoff), 0.6), 
			0.8 + p_beacher.get_noise_2d(i,j)/2.3 - pow((peninsuler.get_noise_2d(i,j) - peninsuler_cutoff), 0.6))
			
			
			var beach: bool = coastness > beach_cutoff
			
			var lake: bool = laker.get_noise_2d(i,j)/1.3 + 1 - pow(coastness, 0.6) > lake_cutoff
			
			var tiles_to_place: Array[Dictionary] = []
			
			
			
			
			match [continental, peninsuler_caved, away_from_coast, lake, beach]:
				
				[true, false, false, _, true]:#beach
					#atlas_id = 2; atlas_tile = Vector2i(1,0)
					tiles_to_place.append({"atlas_id": 2})
					
				[true, false, true, true, ..]:#lake
					#atlas_id = 1; atlas_tile = Vector2i(9, 0)
					tiles_to_place.append({"atlas_id": 1, "tile": Vector2i(9, 0)})
				#[true, true, ..]: tiles_to_place.append({"atlas_id": 0, "tile": Vector2i(10, 1)})
				[false, ..], [true, true, ..]:#ocean
					#atlas_id = 1; atlas_tile = Vector2i.ZERO
					tiles_to_place.append({"atlas_id": 1, "tile": Vector2i(0, 0)})
					#TODO HACER ANIMATED WATER
				
				[true, false, ..]:#grass
					#atlas_id = 0; atlas_tile = Vector2i(0,1)
					tiles_to_place.append({"atlas_id": 0, "tile": Vector2i(0, 1)})
					
					#tiles_to_place.append({"coords" = })
			
			for tile in tiles_to_place:
				var coords: Vector2i = Vector2i(center.x + i, center.y + j)
				var layer: int = 0
				var atlas_id: int = 0
				var atlas_tile: Vector2i= Vector2i.ZERO
				if tile.has("displacement"):
					coords += tile["displacement"]
				if tile.has("layer"):
					coords = tile["layer"]
				if tile.has("atlas_id"):
					atlas_id = tile["atlas_id"]
				if tile.has("tile"):
					atlas_tile = tile["tile"]
					
				set_cell(layer, coords, atlas_id, atlas_tile)
				
			
			
	set_cell(0, Vector2i.ZERO, 1 , Vector2i(0, 9))
	

func calculate_border_closeness_factor(index_value: int, size: int) -> float:
	return abs(index_value)/(size/float(2))
