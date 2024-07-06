extends Node
# PONER EN LA GUI ARRIBA A LA DERECHA DAYS SURVIVED: X, EN FUENTE DIABLESCA

@onready var tile_map: GdTileMap = $GdTileMap

func _ready() -> void:
	if Config.insta_start:
		start_new_game([{}], [1])

func start_new_game(players_start_data: Array, peers: PackedInt32Array) -> void:
	
	tile_map.generate_world.rpc()
	
	var i: int = 0
	for player_start_data: Dictionary in players_start_data:
		
		if not player_start_data.has(Keys.NAME):
			player_start_data[Keys.NAME] = "nameless_player%d"%i
		if not player_start_data.has(Keys.RACE):
			player_start_data[Keys.RACE] = &"controllable_random"
		if not player_start_data.has(Keys.KLASS):
			player_start_data[Keys.KLASS] = &"random"
		if not player_start_data.has(Keys.SEX):
			player_start_data[Keys.SEX] = &"random"
		if not player_start_data.has(Keys.HEAD):
			player_start_data[Keys.HEAD] = &"random"
		if not player_start_data.has(Keys.BODY):
			player_start_data[Keys.BODY] = &"random"
		
		player_start_data[Keys.FACTION] = &"player"	
		
		#extra health para los protagonists
		player_start_data[Keys.HEALTH_MULTIP] = 2	
		
		var player_being_preinit_data = BeingStatePreIniter.new()
		player_being_preinit_data.construct(player_start_data)
		
		var being: Being = tile_map.spawn_starting_player(player_being_preinit_data)
		
		await get_tree().create_timer(0.3).timeout#SI NO SE HACE ESTO SE LE SACA LA AUTHORITY AL HOST ANTES DE Q PUEDA PONERLE LA POSITION
		being.give_control.rpc(peers[i])
		
		i+=1
