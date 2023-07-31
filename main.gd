extends Node

var player_id: String = "nameless_%s" % generateRandomString(4)

@onready var menu_container: Container = $Canvas/MenuContainer

func _ready() -> void:
	_update_player_id_for_children()

func _on_main_menu_name_changed(new_name: String) -> void:
	player_id = new_name
	_update_player_id_for_children()
	
func _update_player_id_for_children():
	menu_container.update_player_id(player_id)

func generateRandomString(length: int) -> String:
	var chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	var random_string = ""
	for i in range(length):
		var random_index = randi() % chars.length()
		random_string += chars.substr(random_index, 1)
	return random_string
	


	
