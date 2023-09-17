extends RefCounted
# convertir a C#? https://ask.godotengine.org/56851/c%23-how-to-create-godot-collections-dictionary

static var _world: Array[Array]

#en extra info se pueden meter params para la randomización de parámetros
static func generate(world: Array[Array], tile_picker: Script, center: Vector2i, size: Vector2i, extra_info: Dictionary, seed: int = 0) -> void:
	#in extra_info se pasan noise maps globales al mundo, parámatros de configuración de la generation, etc
	
	_world = world
	
	var continenter: FastNoiseLite = FastNoiseLite.new()
	var peninsuler: FastNoiseLite = FastNoiseLite.new()
	var laker: FastNoiseLite = FastNoiseLite.new()
	var c_beacher: FastNoiseLite = FastNoiseLite.new()
	var p_beacher: FastNoiseLite = FastNoiseLite.new()

	continenter.seed = seed
	peninsuler.seed = seed
	laker.seed = seed
	c_beacher.seed = seed
	p_beacher.seed = seed + 1
	
	var rng = RandomNumberGenerator.new()
	rng.seed = seed
	#usar para randomizar (dentro de un rango sensato) los números de configuración de la generación (generar entre 0,8 y 1,2)
	
	continenter.noise_type = FastNoiseLite.TYPE_SIMPLEX
	continenter.frequency = 0.14/size.length()
	
	continenter.fractal_lacunarity = 2.8
	continenter.fractal_weighted_strength = 0.5
	var continental_cutoff: float = 0.6 * pow(size.length()/1600, 0.05)

	peninsuler.noise_type = FastNoiseLite.TYPE_SIMPLEX
	peninsuler.frequency = 5/size.length()
	peninsuler.fractal_gain = 0.56
	const peninsuler_cutoff: float = -0.1
	
	c_beacher.noise_type = FastNoiseLite.TYPE_SIMPLEX_SMOOTH
	c_beacher.frequency = 5/size.length()
	const beach_cutoff: float = 0.8
	
	p_beacher.noise_type = FastNoiseLite.TYPE_SIMPLEX_SMOOTH
	p_beacher.frequency = 10/size.length()
	p_beacher.fractal_octaves = 1
	
	laker.frequency = 15/size.length()
	const lake_cutoff: float = 0.5
	
	while continenter.get_noise_2dv(center) < continental_cutoff + 0.13:
		continenter.offset.x += 3
		continenter.offset.y += 3
	
	_place_dungeon_entrances(size, center, rng, continenter, continental_cutoff, peninsuler, peninsuler_cutoff, laker, lake_cutoff)
	
	for i in range(-size.x/2, size.x/2):
		for j in range(-size.y/2, size.y/2):
			
			var bcf: float = _get_bcf(i, j, size, center)
			
			var continentness: float = _get_continentness(i, j, continenter, bcf)
			
			var continental: bool = continentness > continental_cutoff
			var peninsuler_caved: bool = peninsuler.get_noise_2d(i,j) < peninsuler_cutoff
			
			var away_from_coast: bool = continentness > continental_cutoff + 0.05 and peninsuler.get_noise_2d(i,j) > peninsuler_cutoff + 0.27
			
			var beachness: float = _get_beachness(i, j, c_beacher, continentness, continental_cutoff, p_beacher, peninsuler, peninsuler_cutoff)
			
			var beach: bool = beachness > beach_cutoff
			
			var lake: bool = laker.get_noise_2d(i,j)/1.3 + 1 - pow(beachness, 0.6) > lake_cutoff
			
			var info: Dictionary = {
				&"continental": continental,
				&"peninsuler_caved": peninsuler_caved,
				&"away_from_coast": away_from_coast,
				&"lake": lake,
				&"beach": beach
			}
			_place_tiles(Vector2i(i, j) + center, tile_picker.get_tiles(info))
			
static func _place_tiles(coords: Vector2i, tiles_to_place: Array[StringName]) -> void:
	for tile_id in tiles_to_place:
		var tile: Tile = GlobalData.tiles[tile_id]
		(_world[coords.x + _world.size()/2][coords.y + _world[0].size()/2] as Array).append(tile)
		
static func _place_dungeon_entrances(size: Vector2i, center: Vector2i, rng: RandomNumberGenerator,
	continenter: FastNoiseLite, continental_cutoff: float, 
	peninsuler: FastNoiseLite, peninsuler_cutoff: float,
	laker: FastNoiseLite, lake_cutoff: float):
	
	var ri: int = rng.randi_range(-size.x/2, size.x/2)
	var rj: int = rng.randi_range(-size.y/2, size.y/2)
	var dungeon_coords: Array[Vector2i]
	
	var tries: int = 0
	var min_distance_mult: float = 1
	
	while dungeon_coords.size() < 3:
		ri = rng.randi_range(-size.x/2, size.x/2)
		rj = rng.randi_range(-size.y/2, size.y/2)
		
		tries +=1
		if (_get_continentness(ri, rj, continenter, _get_bcf(ri, rj, size, center)) > continental_cutoff + 0.032 
		and peninsuler.get_noise_2d(ri, rj) > peninsuler_cutoff + 0.1 and laker.get_noise_2d(ri,rj) < lake_cutoff - 0.04):
			
			var far_from_dungeons: bool = true
			for coord in dungeon_coords:
				if Vector2(ri, rj).distance_to(coord) < size.length()* 0.25 * min_distance_mult:
					far_from_dungeons = false
					break
				
			if far_from_dungeons:
				_place_tiles(center + Vector2i(ri, rj), [&"cave_mossy"])
				dungeon_coords.append(Vector2i(ri, rj))
				print(Vector2i(ri, rj))
			else:
				min_distance_mult = clampf(1500 / float(tries), 0, 1)

static func _get_bcf(i: int, j: int, size: Vector2i, center:Vector2i) -> float:
	return max(abs(i - center.x)/(size.x/float(2)), abs(j - center.y)/(size.y/float(2)),)

static func _get_continentness(i: int, j: int, continenter: FastNoiseLite, bcf: float) -> float:
	return continenter.get_noise_2d(i,j) - pow(bcf, 43) - bcf/4.7

static func _get_beachness(i: int, j: int, c_beacher: FastNoiseLite, continentness: float, continental_cutoff: float, 
	p_beacher: FastNoiseLite, peninsuler: FastNoiseLite, peninsuler_cutoff: float) -> float:
	return maxf(
		0.72 + c_beacher.get_noise_2d(i,j)/2.3 - pow((continentness - continental_cutoff), 0.55),  
		0.8 + p_beacher.get_noise_2d(i,j)/2.3 - pow((peninsuler.get_noise_2d(i,j) - peninsuler_cutoff), 0.6))
