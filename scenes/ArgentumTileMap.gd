extends ArgentumTileMap
class_name GdTileMap

var _beings: Dictionary # key(str): individual unique id. value: Being. el multiplayerspawner se encarga del sync
var tiles_states: Dictionary # key: posx_posy_zi. value: state object

func _setup_config():
	#self.tiles_data = GlobalData.tiles
	self.tile_set = preload("res://resources/world/tile_set.tres")
	add_layer(0);add_layer(1);add_layer(2)
	set_layer_z_index(2, 10)
	set_layer_y_sort_enabled(2, true)
	y_sort_enabled = true

func _ready():
	_setup_config()
func _process(_delta):
	pass
	

func generate_world():
	
	const WORLD_SIZE: Vector2i = Vector2i(5000, 5000)
	assert(WORLD_SIZE.x > 500 && WORLD_SIZE.y >500)
	
	generate_world_matrix(WORLD_SIZE, GlobalData.tiles)
	
	var fcg: FracturedContinentGenerator = FracturedContinentGenerator.new()
	
	generate_formation(fcg, Vector2i.ZERO, WORLD_SIZE, GlobalData.tile_selections["temperate"], 3333, {})
	
	_players_start_position = WORLD_SIZE/2
	# FIXME HACER CHECK DE SI EL SPAWN ESTÁ FUERA DEL WORLD CON set: DE GDSCRIPT
	# ALERT SI APARECE TODO VACÍO PUEDE SER PORQUE EL SPAWN POINT ESTÁ PUESTO EN UN LUGAR VACÍO

#region SPAWNING 
var _players_start_position: Vector2i

var _player_i: int = 0
func spawn_starting_player(being: Being):
	birth_being_at(being, _players_start_position + Vector2i(_player_i, 0), true)
	_player_i += 1
	

var _birthed_beings_i: int = 0
func birth_being_at(being: Being, glb_coords: Vector2, player: bool = false):
	being.uid = _birthed_beings_i
	var local_coords: Vector2i = map_to_local(glb_coords)
	#está mal el add_child, solo deberia usarse el add_child si está en una tile cargada
	
	if player or get_cell_tile_data(0, local_coords):
		add_child(being)
		_birthed_beings_i += 1
		being.position = local_coords
		being.z_index = 10
	else:
		freeze_and_store_being(glb_coords, being.uid)

#endregion SPAWNING

func _free_and_store_being():
	pass
	
