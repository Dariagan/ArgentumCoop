extends Control

@onready var main_menu: Control = $MainMenu


@export var lobby_interface_scene: PackedScene
var lobby_interface: LobbyInterface


signal lobby_started(lobby_interface: LobbyInterface, joined_ip: String)


func _ready() -> void:
	if GlobalData.insta_start:
		hide()
	

func _on_lobby_player_list_updated(players: Array) -> void:
	if is_instance_valid(lobby_interface):
		lobby_interface.update_player_list(players)


func _on_main_menu_host_pressed() -> void:
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby_started.emit(lobby_interface, "")
	lobby_interface.player_clicked_leave.connect(_return_from_lobby_to_menu, CONNECT_ONE_SHOT)


func _on_main_menu_join_pressed(ip: String) -> void:
	
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby_started.emit(lobby_interface, ip)
	lobby_interface.player_clicked_leave.connect(_return_from_lobby_to_menu, CONNECT_ONE_SHOT)
	

func _return_from_lobby_to_menu() -> void:
	lobby_interface.queue_free()
	main_menu.show()
	
func _on_lobby_removed_from_lobby(_kicked: bool) -> void:
	_return_from_lobby_to_menu()
# for joiner
