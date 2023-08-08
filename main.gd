extends Node

var username: String = "nameless_%s" % generate_random_string(4)

@onready var menu_control: Container = $Canvas/MenuControl
@onready var lobby: Node = $Lobby


func _ready() -> void:
	_update_username_for_children()

func _on_main_menu_name_changed(new_name: String) -> void:
	username = new_name
	_update_username_for_children()
	
func _update_username_for_children():
	#volver un grupo
	lobby.update_username(username)

func generate_random_string(length: int) -> String:
	var chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	var random_string = ""
	for i in range(length):
		var random_index = randi() % chars.length()
		random_string += chars.substr(random_index, 1)
	return random_string
		
