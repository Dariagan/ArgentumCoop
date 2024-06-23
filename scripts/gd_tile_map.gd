extends RustTileMap
class_name GdTileMap

var _beings: Dictionary # key(str): individual unique id. value: Being Scene. el multiplayerspawner se encarga del sync
var tiles_states: Dictionary # key: posx_posy_zi (vec3, no un string). value: state object
const WORLD_SIZE: Vector2i = Vector2i(2500, 2500)

# IMPORTANTE: USAR CUSTOM DATA DE TILE EN TILESET PA PONER DATOS DE LA TILE, ASÍ ES FÁCILMENTE ACCESIBLE DESDE EL GDSIDE

func _ready():
	_setup_config()
	
func _setup_config():
	self.tile_set = preload("res://resource_instances/tiling/tile_set.tres")
	add_layer(0);add_layer(1);add_layer(2)
	set_layer_z_index(2, 10)
	set_layer_y_sort_enabled(2, true)
	y_sort_enabled = true


func _process(_delta):
	pass


@rpc("call_local")
func generate_world():
	assert(WORLD_SIZE.x > 500 && WORLD_SIZE.y >500)
	
	var tiles: Array[Tile] = []
	tiles.append_array(GlobalData.tiles_data.values())
		
	generate_world_matrix(WORLD_SIZE, tiles)
	
	generate_formation(0, Vector2i.ZERO, WORLD_SIZE, GlobalData.tile_selections[&"temperate"], 5, {})
	
	_players_start_position = WORLD_SIZE/2
	# FIXME HACER CHECK DE SI EL SPAWN ESTÁ FUERA DEL WORLD CON set: DE GDSCRIPT
	# ALERT SI APARECE TODO VACÍO PUEDE SER PORQUE EL SPAWN POINT ESTÁ PUESTO EN UN LUGAR VACÍO
	
	if multiplayer.get_unique_id() == 1:
		await get_tree().create_timer(2).timeout
		birth_beingkind_at_snapped(&"basic_warrior", &"wild", WORLD_SIZE/2 + Vector2i.ONE*2)

#region SPAWNING 
var _players_start_position: Vector2i

var _player_i: int = -1
#TODO buscar suitable tiles
func spawn_starting_player(preinitdata: BeingStatePreIniter) -> Being:
	_player_i += 1
	return birth_being_snapped_at(preinitdata, _players_start_position + Vector2i(_player_i, 0), true)

var _birthed_beings_i: int = 0
#ALERT, NO APARECE EL BEING SI LA TILE NO ESTÁ CARGADA EN EL MOMENTO Q SPAWNEA
func birth_being_snapped_at(preinitdata: BeingStatePreIniter, tilemap_coords: Vector2i, player: bool = false) -> Being:
	return birth_being_at(preinitdata, map_to_local(tilemap_coords), player)
func birth_being_at(preinitdata: BeingStatePreIniter, loc_coords: Vector2, player: bool = false) -> Being:
	
	var being: Being = preload("res://scenes/being.tscn").instantiate()
	#nota: el being.name hay q ponerlo antes del add_child
	add_child(being, true)
	being.uid = _birthed_beings_i; _birthed_beings_i += 1
	being.construct(preinitdata)
	
	if player or get_cell_tile_data(0, local_to_map(loc_coords)):
		being.position = loc_coords
		being.z_index = 10
		return being
	else:
		_beings[being.uid] = being.serialize() # no sé si hacer esto o guardar packedscene del being
		being.queue_free()
		#freeze_and_store_being(local_to_map(loc_coords), being.uid)
		return null

func birth_beingkind_at_snapped(beingkind_id: StringName, faction: StringName, map_coords: Vector2i) -> Being:
	return birth_beingkind_at(beingkind_id, faction, map_to_local(map_coords))
func birth_beingkind_at(beingkind_id: StringName, faction: StringName, loc_coords: Vector2) -> Being:
	assert(GlobalData.beingkinds.has(beingkind_id))
	var beingkind: BeingKind = GlobalData.beingkinds[beingkind_id]
	
	return birth_being_at(beingkind.instantiate(faction), loc_coords)
#endregion SPAWNING

	


func _on_tile_unloaded(coords):
	pass
