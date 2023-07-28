extends TileMap


func spawn_player(character: Character):
	call_deferred("add_child", character)


