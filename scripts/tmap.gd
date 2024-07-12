extends RustTileMap
class_name GdTileMap
var _beings: Dictionary # key(str): individual unique id. value: Being Scene. el multiplayerspawner se encarga del sync
var tiles_states: Dictionary # key: posx_posy_zi (vec3, no un string). value: state object
const WORLD_SIZE: Vector2i = Vector2i(2500, 2500)


#IMPORTANTE: USAR CUSTOM DATA DE TILE EN TILESET PA PONER DATOS DE LA TILE, ASÍ ES FÁCILMENTE ACCESIBLE DESDE EL GDSIDE

const TILE_SET: TileSet = preload("res://resource_instances/tiling/tset.tres")

@onready var water_sprite: Sprite2D = $WaterSprite



var tile_id_binded_layers: Dictionary = {} #key: tile_id . val: TileMapLayer
var beings_z_index: int = -1
#TODO pasarlo a rust cuando agreguen TileMapLayer
var zlevel_layers: Array[TileMapLayer] = []
# ALERT SÍ HACE FALTA TOOL PORQUE TIENE QUE ESTAR PRELOADED ANTES DE QUE SUCEDA MULTIPLAYER. ADD_CHILD NO SIRVE PORQUE LOS RPC NO FUNCIONARIAN
func _add_tile_map_layers(layer_names: Array[StringName]) -> void:
	var i: int = 0
	for layer_name in layer_names:
		var new_child = TileMapLayer.new()
		new_child.name = layer_name 
		add_child(new_child); move_child(new_child, i); new_child.z_index = i
		new_child.rendering_quadrant_size = 20
		new_child.tile_set = TILE_SET
		zlevel_layers.append(new_child) 
		if layer_name == &"Structure":
			new_child.y_sort_enabled = true	
			beings_z_index = i-1
		i+=1
	water_sprite.z_index = Enums.TileZLevel.Water

func _process(delta):
	pass

@rpc("call_local")
func generate_world():
	@warning_ignore("assert_always_true")
	assert(WORLD_SIZE.x > 500 && WORLD_SIZE.y > 500)
	
	var tiles: Array[Tile] = []
	tiles.append_array(Global.tilesdict.values())
		
	generate_world_matrix(WORLD_SIZE, tiles)
	
	generate_formation(0, Vector2i.ZERO, WORLD_SIZE, Global.tile_selections[&"selectiontemperate"], 0, {})
	
	_players_start_position = WORLD_SIZE/2
	# FIXME HACER CHECK DE SI EL SPAWN ESTÁ FUERA DEL WORLD CON set: DE GDSCRIPT
	# ALERT SI APARECE TODO VACÍO PUEDE SER PORQUE EL SPAWN POINT ESTÁ PUESTO EN UN LUGAR VACÍO
	#water_sprite.show()
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
			
	if player or zlevel_layers[0].get_cell_tile_data(local_to_tilemap(loc_pos)): #TODO
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

func tilemap_to_local(tilemap_pos: Vector2i) -> Vector2: return zlevel_layers[0].map_to_local(tilemap_pos)

func local_to_tilemap(local_pos: Vector2) -> Vector2i: return zlevel_layers[0].local_to_map(local_pos)


func _on_tile_unloaded(coords):
	pass
