extends Node

func _enter_tree() -> void:
	GlobalData.username = "nameless_%s" % _generate_random_string(4)
	
func _on_main_menu_name_changed(new_name: String) -> void:
	GlobalData.username = new_name

func _generate_random_string(length: int) -> String:
	var chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	var random_string = ""
	for i in range(length):
		var random_index = randi() % chars.length()
		random_string += chars.substr(random_index, 1)
	return random_string
