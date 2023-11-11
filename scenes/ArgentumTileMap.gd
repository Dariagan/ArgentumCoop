extends ArgentumTileMap
class_name GdTileMap

func _setup_config():
	self.tiles_data = GlobalData.tiles
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
	
	generate_world_matrix(WORLD_SIZE)
	
	var fcg: FracturedContinentGenerator = FracturedContinentGenerator.new()
	generate_formation(fcg, Vector2i.ZERO, Vector2i(5000,5000), GlobalData.tile_selections["temperate"], 4, {})
		
	
	_players_start_position = WORLD_SIZE/2
	# FIXME HACER CHECK DE SI EL SPAWN ESTÁ FUERA DEL WORLD CON set: DE GDSCRIPT
	# OJO SI APARECE TODO VACÍO PUEDE SER PORQUE EL SPAWN POINT ESTÁ DESPLAZADO

#region SPAWNING 
var _players_start_position: Vector2i

var player_i: int = 0
func spawn_starting_player(being: Being):
	spawn_being_at(being, _players_start_position + Vector2i(player_i, 0))
	player_i += 1

func spawn_being_at(being: Being, pos: Vector2i):
	add_child(being);
	being.position = map_to_local(pos)
	being.z_index = 10

#endregion SPAWNING
