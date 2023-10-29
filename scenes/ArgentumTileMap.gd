extends ArgentumTileMap


# Called when the node enters the scene tree for the first time.
func _ready():
	self.tiles_data = GlobalData.tiles
	add_layer(0);add_layer(1);add_layer(2)
	set_layer_z_index(2, 10)
	set_layer_y_sort_enabled(2, true)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func generate_world():
	generate_world_matrix(Vector2i(4500, 4500))
	var fcg: FracturedContinentGenerator = FracturedContinentGenerator.new()
	generate_formation(fcg, Vector2i.ZERO, Vector2i(4000,4000), FormationGenerator.TEMPERATE, 0, {})
	
