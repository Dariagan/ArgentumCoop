extends Node
# PONER EN LA GUI ARRIBA A LA DERECHA DAYS SURVIVED: X, EN FUENTE DIABLESCA

@onready var tile_map: GdTileMap = $ArgentumTileMap

func _ready() -> void:
	if GlobalData.insta_start:
		start_new_game([{}], [1])



func start_new_game(players_start_data: Array, peers: Array) -> void:
	
	#TODO hacer un subviewport de tamaño fijo para el game, y poner gui en los costados
	#TODO CONFIGURAR ESTO MANULMENTE? 
	#DisplayServer.screen_get_size()
	get_tree().root.size = Vector2i(900, 500)
	get_tree().root.content_scale_mode = Window.CONTENT_SCALE_MODE_CANVAS_ITEMS
	get_tree().root.content_scale_aspect = Window.CONTENT_SCALE_ASPECT_KEEP_HEIGHT
	get_tree().root.content_scale_stretch = Window.CONTENT_SCALE_STRETCH_FRACTIONAL
	"""get_tree().root.content_scale_mode = Window.CONTENT_SCALE_MODE_VIEWPORT
	get_tree().root.content_scale_aspect = Window.CONTENT_SCALE_ASPECT_KEEP
	get_tree().root.content_scale_stretch = Window.CONTENT_SCALE_STRETCH_FRACTIONAL #nota: esto afecta al viewport stretching"""

	tile_map.generate_world.rpc()
	
	var i: int = 0
	for player_start_data: Dictionary in players_start_data:
		
		if not player_start_data.has(Constants.KEYS.NAME):
			player_start_data[Constants.KEYS.NAME] = "nameless_player%d"%i
		if not player_start_data.has(Constants.KEYS.RACE):
			player_start_data[Constants.KEYS.RACE] = &"controllable_random"
		if not player_start_data.has(Constants.KEYS.KLASS):
			player_start_data[Constants.KEYS.KLASS] = &"random"
		if not player_start_data.has(Constants.KEYS.SEX):
			player_start_data[Constants.KEYS.SEX] = &"random"
		if not player_start_data.has(Constants.KEYS.HEAD):
			player_start_data[Constants.KEYS.HEAD] = &"random"
		if not player_start_data.has(Constants.KEYS.BODY):
			player_start_data[Constants.KEYS.BODY] = &"random"
		
		player_start_data[Constants.KEYS.FACTION] = &"player"	
		
		#extra health para los protagonists
		player_start_data[Constants.KEYS.EXTRA_HEALTH_MULTI] = 2	
		
		var player_being_preinit_data = BeingStatePreIniter.new()
		player_being_preinit_data.construct(player_start_data)
		
		var being: Being = tile_map.spawn_starting_player(player_being_preinit_data)
		
		await get_tree().create_timer(0.0001).timeout
		being.give_control.rpc(peers[i])
		
		i+=1
		
	
