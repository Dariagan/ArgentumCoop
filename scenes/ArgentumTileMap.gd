extends ArgentumTileMap
class_name GdTileMap

var _beings: Dictionary # key(str): individual unique id. value: Being Scene. el multiplayerspawner se encarga del sync
var tiles_states: Dictionary # key: posx_posy_zi (vec3, no un string). value: state object

# IMPORTANTE: USAR CUSTOM DATA DE TILE EN TILESET PA PONER DATOS DE LA TILE, ASÍ ES FÁCILMENTE ACCESIBLE DESDE EL GDSIDE

func _setup_config():
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

var _player_i: int = -1
#TODO buscar suitable tiles
func spawn_starting_player(preinitdata: BeingStatePreIniter) -> Being:
	_player_i += 1
	return await birth_being_snapped_at(preinitdata, _players_start_position + Vector2i(_player_i, 0), true)

var _birthed_beings_i: int = 0
func birth_being_snapped_at(preinitdata: BeingStatePreIniter, tilemap_coords: Vector2i, player: bool = false) -> Being:
	return await birth_being_at(preinitdata, map_to_local(tilemap_coords), player)

func birth_being_at(preinitdata: BeingStatePreIniter, loc_coords: Vector2, player: bool = false) -> Being:
	
	var being: Being = preload("res://scenes/being.tscn").instantiate()
	add_child(being)
	being.uid = _birthed_beings_i; _birthed_beings_i += 1
	being.construct(preinitdata)
	
	if player or get_cell_tile_data(0, local_to_map(loc_coords)):
		being.position = loc_coords
		being.z_index = 10
		return being
	else:
		_beings[being.uid] = being.serialize()
		freeze_and_store_being(loc_coords, being.uid)
		return null
		
func birth_beingkind_at(beingkind_id: StringName, loc_coords: Vector2) -> Being:
	assert(GlobalData.beingkinds.has(beingkind_id))
	var beingkind: BeingKind = GlobalData.beingkinds[beingkind_id]
	var preinitdata: BeingStatePreIniter = beingkind.instantiate()
	return await birth_being_at(preinitdata, loc_coords)
#endregion SPAWNING

	
