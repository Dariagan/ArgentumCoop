extends ArgentumTilemap


# Called when the node enters the scene tree for the first time.
func _ready():
	tiles_data = GlobalData.tiles
	add_layer(0);add_layer(1);add_layer(2)
	set_layer_z_index(2, 10)
	set_layer_y_sort_enabled(2, true)
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
