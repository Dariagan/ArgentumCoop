extends RustTileMap
class_name GdTileMap
var mbeings: Dictionary # key(str): individual unique id. value: Being Scene. el multiplayerspawner se encarga del sync
var mtiles_states: Dictionary[Vector3, Dictionary] # key: posx_posy_zi (vec3, no un string). value: state object
const WORLD_SIZE: Vector2i = Vector2i(500, 500)

#IMPORTANTE: USAR CUSTOM DATA DE TILE EN TILESET PA PONER DATOS DE LA TILE, ASÍ ES FÁCILMENTE ACCESIBLE DESDE EL GDSIDE

var tile_id_binded_layers: Dictionary = {} #key: tile_id . val: TileMapLayer

var mbeinggentempls_to_b_spawned: Dictionary[Vector2i, StringName] = {}; var mbeingtempls_fac_ids: Dictionary[Vector2i, StringName] = {}

#don't define ready func

@rpc("call_local")
func generate_world(world_config: Dictionary = {}):#
	@warning_ignore("assert_always_true")
	assert(WORLD_SIZE.x >= 500 && WORLD_SIZE.y >= 500)
	
	var tiles: Array[Tile] = []; tiles.append_array(Global.tilesdict.values())
		
	generate_world_matrix(WORLD_SIZE, tiles)
	
	generate_formation(0, Vector2i.ZERO, WORLD_SIZE, Global.tile_selections[&"selectiontemperate"], 0, {})
	
	mplayers_start_position = WORLD_SIZE/2
	# FIXME HACER CHECK DE SI EL SPAWN ESTÁ FUERA DEL WORLD CON set: DE GDSCRIPT
	# ALERT SI APARECE TODO VACÍO PUEDE SER PORQUE EL SPAWN POINT ESTÁ PUESTO EN UN LUGAR VACÍO
	#water_sprite.show()
	
	
	if is_host():
		do_natural_spawning()
	
#region SPAWNING 
var mplayers_start_position: Vector2i

var mplayer_i: int = -1
#TODO buscar suitable tiles
func spawn_starting_player(preinit: BeingPreInit, peer_id: int) -> Being:
	mplayer_i += 1
	return birth_being_snapped_at(preinit, mplayers_start_position + Vector2i(mplayer_i*2, 0), true, peer_id)

var mbirthed_beings_i: int = 0
#ALERT, NO APARECE EL BEING SI LA TILE NO ESTÁ CARGADA EN EL MOMENTO Q SPAWNEA
func birth_being_snapped_at(preinit: BeingPreInit, tilemap_gridposs: Vector2i, isplayerfac:bool=false,mp_auth:int=1) -> Being:
	return birth_being_at(preinit, tilemap_to_local(tilemap_gridposs), isplayerfac, mp_auth)
func birth_being_at(preinit: BeingPreInit, loc_pos: Vector2, isplayerfac:bool=false, mp_auth:int=1, master:Being=null) -> Being:#, scene:String="res://scenes/being.tscn" requires load() instead of preload which is much slower. should be preloaded before
	var being: Being = preload("res://scenes/being.tscn").instantiate()
	#nota: el being.name hay q ponerlo antes del add_child
	being.name = str(mbirthed_beings_i)
	add_child(being); being.setsync_zindex.rpc(beings_z_index)
	being.construct(preinit, mbirthed_beings_i); mbirthed_beings_i += 1
	being.setsync_pos_reliable.rpc(loc_pos)
	if master != null:
		set_master_follower.rpc(master.get_path(), being.get_path())
		if mp_auth != 1: being.setsync_mp_authority.rpc(mp_auth)
	
	if preinit.mfollowers.size() > 0:
		for follower_template in preinit.mfollowers:
			var spawned_follower: Being = birth_being_at(follower_template.instantiate(\
				preinit.mfaction.minstance_id),loc_pos, isplayerfac, mp_auth, being)
			
	if isplayerfac or zlevel_layers[0].get_cell_tile_data(local_to_tilemap(loc_pos)): #TODO
		being.set_multiplayer_authority(mp_auth)
		return being
	else:#TODO almacenar la authority?
		return null
		mbeings[being.uid] = being.serialize() # no sé si hacer esto o guardar packedscene del being
		being.queue_free()
		#freeze_and_store_being(local_to_tilemap(loc_pos), being.uid)
		return null
	
		
@rpc("call_local")
func set_master_follower(master_name: NodePath, follower_name: NodePath):
	var master: Being = get_node(master_name)
	var follower: Being = get_node(follower_name)
	master.mistate.mfollowers.append(follower)
	follower.mistate.mmaster = master

func mass_birth_being_gen_template_at_snapped(being_gen_template_ids: Array[StringName], spawns_gridposs: Array[Vector2i], faction_ids: Array[StringName]):
	for i in being_gen_template_ids.size():
		mbeinggentempls_to_b_spawned[spawns_gridposs[i]] = being_gen_template_ids[i]
		mbeingtempls_fac_ids[spawns_gridposs[i]] = faction_ids[i]

func birth_being_gen_template_at_snapped(being_gen_template_id: StringName, map_gridposs: Vector2i, faction: StringName, mp_auth:int=1) -> Being:
	return birth_being_gen_template_at(being_gen_template_id, faction, tilemap_to_local(map_gridposs), mp_auth)
func birth_being_gen_template_at(being_gen_template_id: StringName, faction: StringName, loc_pos: Vector2,mp_auth:int=1) -> Being:
	var being_gen_template: BeingGenTemplate = Global.being_gen_templates[being_gen_template_id]
	
	return birth_being_at(being_gen_template.instantiate(faction), loc_pos, false, mp_auth)
	
func activate_beingtempl_at_grid_pos(gridpos: Vector2i):
	birth_being_gen_template_at_snapped(mbeinggentempls_to_b_spawned[gridpos], gridpos, mbeingtempls_fac_ids[gridpos])
	mbeinggentempls_to_b_spawned.erase(gridpos); mbeingtempls_fac_ids.erase(gridpos)
#endregion SPAWNING

func tilemap_to_local(tilemap_pos: Vector2i) -> Vector2: return zlevel_layers[0].map_to_local(tilemap_pos)

func local_to_tilemap(local_pos: Vector2) -> Vector2i: return zlevel_layers[0].local_to_map(local_pos)

var msoiltiles_states: Dictionary[Vector2i, Dictionary] = {}
var mwatertiles_states: Dictionary[Vector2i, Dictionary] = {}
var mfloortiles_states: Dictionary[Vector2i, Dictionary] = {}
var mstaintiles_states: Dictionary[Vector2i, Dictionary] = {}
var mstructuretiles_states: Dictionary[Vector2i, Dictionary] = {}
var mrooftiles_states: Dictionary[Vector2i, Dictionary] = {}
var malltiles_states_arr: Array = [msoiltiles_states, mwatertiles_states, mfloortiles_states, mstaintiles_states, mstructuretiles_states, mrooftiles_states]

func set_tile_state(gridposs: Vector2i, tile_z_level: Enum.TileZLevel, state: Dictionary):
	#EL CLIENTE TAMBIÉN HACE ESTO
	match tile_z_level:
		Enum.TileZLevel.Soil: msoiltiles_states[gridposs] = state
		Enum.TileZLevel.Floor: pass
		Enum.TileZLevel.Stain: pass
		Enum.TileZLevel.Structure: mstructuretiles_states[gridposs] = state
		Enum.TileZLevel.Roof: mrooftiles_states[gridposs] = state
		
#TODO: arreglar set_cell de escenas si lo hace otro jugador antes, si es una escena que no lo haga si ya está cargada
		
func set_cells(grid_positions: Array[Vector2i], z_levels: Array[int], source_atlases: Array[int], atlas_position: Array[Vector2i], _now_loaded_grid_positions: Array):
	var start: float = Time.get_unix_time_from_system()
	for i in grid_positions.size():
		zlevel_layers[z_levels[i]].set_cell(grid_positions[i], source_atlases[i], atlas_position[i])
	tiles_loaded(_now_loaded_grid_positions)
	var end: float = Time.get_unix_time_from_system()
	print(end-start)
	
#LAS ESCENAS-TILE QUE SE CARGUEN TIENEN QUE SER CARGADAS POR TODOS SIMULTÁNEAMENTE??
		
@rpc("any_peer")
func tiles_loaded(positions: Array): 
	if is_host():
		for gridpos: Vector2i in positions:
			if mbeinggentempls_to_b_spawned.has(gridpos):
				activate_beingtempl_at_grid_pos(gridpos)
			for i in LAYER_COUNT:
				if multiplayer.get_remote_sender_id() != multiplayer.get_unique_id() and get_node_at_gridpos(zlevel_layers[i], gridpos) == null:
					#zlevel_layers[i].set_cell(gridpos, )
					pass
				
				var node: Node2D = get_node_at_gridpos(zlevel_layers[i], gridpos)
				if node:#PROBLEMA: EL HOST NO CARGÓ EL node
					show_node.rpc_id(multiplayer.get_remote_sender_id(), node.get_path())
					if node.has_method(&"on_load"):
						node.on_load.rpc()
				if malltiles_states_arr[i].has(gridpos):#PROBLEMA: QUE SE CARGUE UN ESTADO DESACTUALIZADO AL REVISITAR LA TILE. TODO: ELIMINAR DEL DICT ESA KEY O HACER OTRA COSA
					node.load_state.rpc(malltiles_states_arr[i][gridpos])
					malltiles_states_arr[i].erase(gridpos)
					
	else:				
		tiles_loaded.rpc_id(1, positions)	



		
func get_node_at_gridpos(tmap_layer: TileMapLayer, gridpos: Vector2i) -> Node2D:
	for node: Node2D in tmap_layer.get_children():
		if gridpos == local_to_tilemap(node.position):
			return node
	return null

func tile_unloaded(gridpos: Vector2i):
	for i in LAYER_COUNT:
		var node: Node2D = get_node_at_gridpos(zlevel_layers[i], gridpos)
		if node: #and node.hide_on_unload:
			node.hide()
			continue
		zlevel_layers[i].erase_cell(gridpos)
				#node.load_state.rpc(tiles_states[gridpos], gridpos)

@rpc("call_local", "any_peer") func hide_node(nodepath: NodePath) -> void: get_node(nodepath).hide() 
@rpc("call_local", "any_peer") func show_node(nodepath: NodePath) -> void: get_node(nodepath).show() 


func is_host() -> bool: return multiplayer.get_unique_id()==1
