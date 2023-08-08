extends Node
# PONER EN LA GUI ARRIBA A LA DERECHA DAYS SURVIVED: X, EN FUENTE DIABLESCA

@onready var tile_map: TileMap = $TileMap

@export var being_scene: PackedScene
@onready var multiplayer_spawner: MultiplayerSpawner = $MultiplayerSpawner



func start_new_game(players_start_data: Array, peers: Array) -> void:

	var i: int = 0
	for player_start_data in players_start_data:
		player_start_data = player_start_data as Dictionary
		
		if not player_start_data.has("race"):
			player_start_data["race"] = "controllable_random"
		elif not player_start_data.has("sex"):
			player_start_data["sex"] = "random"
		elif not player_start_data.has("klass"):
			player_start_data["klass"] = "random"
		elif not player_start_data.has("head_i"):
			player_start_data["head_i"] = 0
		elif not player_start_data.has("body_i"):
			player_start_data["body_i"] = 0
		
		var being_spawn_data = BeingSpawnData.new(player_start_data)
		
		var being: Being = being_scene.instantiate()
		being.name = str(peers[i])
		
		tile_map.add_child(being)
		
		await get_tree().create_timer(0.4).timeout
		
		being.construct(being_spawn_data)
		being.position.x = i*4
		
		being.cede_authority(peers[i])
		
		i+=1
		
	
