extends TileMap

func spawn_player(character: Being):
	call_deferred("add_child", character)

