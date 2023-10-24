extends Node
# PONER EN LA GUI ARRIBA A LA DERECHA DAYS SURVIVED: X, EN FUENTE DIABLESCA

@onready var tile_map = $ArgentumTilemap

@export var being_scene: PackedScene
@onready var multiplayer_spawner: MultiplayerSpawner = $MultiplayerSpawner

func _ready() -> void:
	if GlobalData.insta_start:
		start_new_game([{}], [1])

func start_new_game(players_start_data: Array, peers: Array) -> void:
	generate_world.rpc()

	var i: int = 0
	for player_start_data in players_start_data:
		player_start_data = player_start_data as Dictionary
		
		if not player_start_data.has("race"):
			player_start_data["race"] = "controllable_random"
		if not player_start_data.has("sex"):
			player_start_data["sex"] = "random"
		if not player_start_data.has("klass"):
			player_start_data["klass"] = "random"
		if not player_start_data.has("head"):
			player_start_data["head"] = "random"
		if not player_start_data.has("body"):
			player_start_data["body"] = "random"
		
		player_start_data["faction"] = "player"	
		
		var being_spawn_data = BeingSpawnData.new(player_start_data)
		
		var being: Being = being_scene.instantiate()
		being.name = str(peers[i])
		being.position.x = i*40
		tile_map.add_child(being)
		being.construct(being_spawn_data)
		
		being.give_control.rpc(peers[i])
		
		i+=1
		
@rpc("call_local")
func generate_world() -> void:

	tile_map.generate_world_matrix(Vector2i(2000, 2000))
	var asd: FracturedContinentGenerator = FracturedContinentGenerator.new()
	tile_map.generate_formation(asd, Vector2i.ZERO, Vector2i(2000,2000), FormationGenerator.TEMPERATE, 0, {})

	#await tile_map.formation_formed
