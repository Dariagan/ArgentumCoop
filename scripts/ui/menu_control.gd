extends Container

@onready var main_menu: Container = $MainMenu

var _username: String

@export var lobby_interface_scene: PackedScene
var lobby_interface: LobbyInterface

signal hosting
signal joining

func _ready() -> void:
	pass
	
func update_username(new_username: String):
	_username = new_username

func _on_main_menu_host_pressed() -> void:
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby_interface.update_username(_username)
	lobby_interface.set_up_host_lobby(_username)
	hosting.emit()



func _on_main_menu_join_pressed() -> void:
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby_interface.update_username(_username)
	lobby_interface.set_up_joiner_lobby()
	joining.emit()



func _on_lobby_player_list_updated(players: Array) -> void:
	lobby_interface.update_player_list(players)
