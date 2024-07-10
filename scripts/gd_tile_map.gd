extends RustTileMap
class_name GdTileMap
var _beings: Dictionary # key(str): individual unique id. value: Being Scene. el multiplayerspawner se encarga del sync
var tiles_states: Dictionary # key: posx_posy_zi (vec3, no un string). value: state object
const WORLD_SIZE: Vector2i = Vector2i(2500, 2500)

var beings_z_index: int = -1

#IMPORTANTE: USAR CUSTOM DATA DE TILE EN TILESET PA PONER DATOS DE LA TILE, ASÍ ES FÁCILMENTE ACCESIBLE DESDE EL GDSIDE


var tileidbinded_layers: Dictionary = {} #key: tile_id . val: TileMapLayer

#TODO pasarlo a rust cuando agreguen TileMapLayer
var zlevelbinded_layers: Array[TileMapLayer] = []
# ALERT SÍ HACE FALTA TOOL PORQUE TIENE QUE ESTAR PRELOADED ANTES DE QUE SUCEDA MULTIPLAYER. ADD_CHILD NO SIRVE PORQUE LOS RPC NO FUNCIONARIAN
func _add_tile_map_layers(layer_names: Array[StringName]):
	var i: int = 0
	for layer_name in layer_names:
		var new_child = TileMapLayer.new()
		new_child.name = layer_name 
		add_child(new_child); move_child(new_child, i); new_child.z_index = i
		new_child.tile_set = preload("res://resource_instances/tiling/tile_set.tres")
		zlevelbinded_layers.append(new_child)
		if layer_name == &"Structure":
			new_child.y_sort_enabled = true	
			beings_z_index = i
		i+=1

func _process(delta):
	pass

func _set_cell_handled(z_level: int, coords: Vector2i, source_atlas: int, atlas_pos: Vector2i, alt_id: int, tile_id: StringName = &"", own_layer: bool = false):
	if not own_layer:
		(get_child(z_level) as TileMapLayer).set_cell(coords, source_atlas, atlas_pos, alt_id)
	elif tileidbinded_layers.has(tile_id):
		(tileidbinded_layers[tile_id] as TileMapLayer).set_cell(coords, source_atlas, atlas_pos, alt_id)
	elif Global.tiles_data.has(tile_id):#crear la layer
		var new_layer: TileMapLayer = TileMapLayer.new(); add_child(new_layer) #ojo, así no esta synqueado el orden de los hijos de tilemap. 
		tileidbinded_layers[tile_id] = new_layer
		new_layer.z_index = z_level
		var tile: Tile = Global.tiles_data[tile_id]
		new_layer.material = Global.shaders[tile.shader_id]
		new_layer.tile_set = preload("res://resource_instances/tiling/tile_set.tres")
		new_layer.set_cell(coords, source_atlas, atlas_pos, alt_id)
	else:
		push_error("banana")
	

@rpc("call_local")
func generate_world():
	@warning_ignore("assert_always_true")
	assert(WORLD_SIZE.x > 500 && WORLD_SIZE.y > 500)
	
	var tiles: Array[Tile] = []
	tiles.append_array(Global.tiles_data.values())
		
	generate_world_matrix(WORLD_SIZE, tiles)
	
	generate_formation(0, Vector2i.ZERO, WORLD_SIZE, Global.tile_selections[&"temperate"], 0, {})
	
	_players_start_position = WORLD_SIZE/2
	# FIXME HACER CHECK DE SI EL SPAWN ESTÁ FUERA DEL WORLD CON set: DE GDSCRIPT
	# ALERT SI APARECE TODO VACÍO PUEDE SER PORQUE EL SPAWN POINT ESTÁ PUESTO EN UN LUGAR VACÍO
	
	if multiplayer.get_unique_id() == 1:
		await get_tree().create_timer(2).timeout
		birth_being_gen_template_at_snapped(&"basic_warrior", &"wild", WORLD_SIZE/2 + Vector2i.ONE*2)
		
#region SPAWNING 
var _players_start_position: Vector2i

var _player_i: int = -1
#TODO buscar suitable tiles
func spawn_starting_player(preinit: BeingStatePreIniter, peer_id: int) -> Being:
	_player_i += 1
	return birth_being_snapped_at(preinit, _players_start_position + Vector2i(_player_i*2, 0), true, peer_id)
	

var _birthed_beings_i: int = 0
#ALERT, NO APARECE EL BEING SI LA TILE NO ESTÁ CARGADA EN EL MOMENTO Q SPAWNEA
func birth_being_snapped_at(preinit: BeingStatePreIniter, tilemap_coords: Vector2i, player:bool=false,mp_auth:int=1) -> Being:
	return birth_being_at(preinit, tilemap_to_local(tilemap_coords), player, mp_auth)
func birth_being_at(preinit: BeingStatePreIniter, loc_pos: Vector2, player:bool=false,mp_auth:int=1) -> Being:
	
	var being: Being = preload("res://scenes/being.tscn").instantiate()
	#nota: el being.name hay q ponerlo antes del add_child
	being.name = str(_birthed_beings_i)
	add_child(being)
	being.z_index = beings_z_index
	being.construct(preinit, _birthed_beings_i); _birthed_beings_i += 1
	
	being.setsync_pos_reliable.rpc(loc_pos)
	
	if preinit.followers.size() > 0:
		for follower_template in preinit.followers:
			var spawned_follower: Being = birth_being_at(follower_template.instantiate(preinit.istate.faction.instance_id),loc_pos,player)
			set_master_follower.rpc(being.get_path(), spawned_follower.get_path())
			
	if player or zlevelbinded_layers[0].get_cell_tile_data(local_to_tilemap(loc_pos)): #TODO
		being.set_multiplayer_authority(mp_auth)
		return being
	else:
		_beings[being.uid] = being.serialize() # no sé si hacer esto o guardar packedscene del being
		being.queue_free()
		#freeze_and_store_being(local_to_tilemap(loc_pos), being.uid)
		return null
		
@rpc("call_local")
func set_master_follower(master_name: NodePath, follower_name: NodePath):
	var master: Being = get_node(master_name)
	var follower: Being = get_node(follower_name)
	master.istate.followers.append(follower)
	follower.istate.master = master

func birth_being_gen_template_at_snapped(being_gen_template_id: StringName, faction: StringName, map_coords: Vector2i,mp_auth:int=1) -> Being:
	return birth_being_gen_template_at(being_gen_template_id, faction, tilemap_to_local(map_coords), mp_auth)
func birth_being_gen_template_at(being_gen_template_id: StringName, faction: StringName, loc_pos: Vector2,mp_auth:int=1) -> Being:
	assert(Global.being_gen_templates.has(being_gen_template_id))
	var being_gen_template: BeingGenTemplate = Global.being_gen_templates[being_gen_template_id]
	
	return birth_being_at(being_gen_template.instantiate(faction), loc_pos, false, mp_auth)
#endregion SPAWNING

func tilemap_to_local(tilemap_pos: Vector2i) -> Vector2: return zlevelbinded_layers[0].map_to_local(tilemap_pos)

func local_to_tilemap(local_pos: Vector2) -> Vector2i: return zlevelbinded_layers[0].local_to_map(local_pos)


func _on_tile_unloaded(coords):
	pass
