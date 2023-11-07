extends ArgentumTileMap
class_name GdTileMap

func _ready():
	self.tiles_data = GlobalData.tiles
	add_layer(0);add_layer(1);add_layer(2)
	set_layer_z_index(2, 10)
	set_layer_y_sort_enabled(2, true)
	y_sort_enabled = true

func _process(_delta):
	pass

func generate_world():

	generate_world_matrix(Vector2i(5000, 5000))
	
	var fcg: FracturedContinentGenerator = FracturedContinentGenerator.new()
	generate_formation(fcg, Vector2i.ZERO, Vector2i(5000,5000), FormationGenerator.TEMPERATE, 40, {})
		
	
	_players_start_position = 2500*Vector2.ONE;

#region Spawning 
var _players_start_position: Vector2i

var player_i: int = 0
func spawn_starting_player(being: Being):
	spawn_being_at(being, _players_start_position + Vector2i(player_i, 0))
	player_i += 1

func spawn_being_at(being: Being, pos: Vector2i):
	add_child(being);
	being.position = map_to_local(pos)
	being.z_index = 10


	
#endregion
