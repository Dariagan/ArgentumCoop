extends Node

func _enter_tree() -> void:
	set_scaling_settings_for_menu_ui()
	GlobalData.username = "nameless_%s" % _generate_random_string(4)

func set_scaling_settings_for_menu_ui() -> void:
	get_tree().root.content_scale_mode = Window.CONTENT_SCALE_MODE_DISABLED
	get_tree().root.content_scale_aspect = Window.CONTENT_SCALE_ASPECT_EXPAND
	
func _on_main_menu_name_changed(new_name: String) -> void:
	GlobalData.username = new_name

static func _generate_random_string(length: int) -> String:
	var chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	var random_string = ""
	for i in range(length):
		var random_index = randi() % chars.length()
		random_string += chars.substr(random_index, 1)
	return random_string;;
