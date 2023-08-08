extends Node
# PONER EN LA GUI ARRIBA A LA DERECHA DAYS SURVIVED: X, EN FUENTE DIABLESCA

@onready var tile_map: TileMap = $TileMap

@export var being_scene: PackedScene

func start_new_game(players_start_data: Array):
	
	for player_start_data in players_start_data:
		var being_spawn_data = BeingSpawnData.new(player_start_data)
		
		var being = being_scene.instantiate()
		
		tile_map.add_child(being)
		
		being.initialize(being_spawn_data)
		
		

