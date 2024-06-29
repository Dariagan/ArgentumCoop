extends Control
class_name UimMenuControl

@onready var main_menu: Control = $MainMenu

@onready var lobby = %Lobby

@export var lobby_interface_scene: PackedScene
var lobby_interface: LobbyInterface

func _ready() -> void:
	if Global.insta_start:
		hide()
		
func _on_main_menu_quick_start_pressed():
	hide()
	lobby.start_new_game()

func _on_main_menu_host_pressed() -> void:
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby._on_menu_control_lobby_started(lobby_interface, "")

func _on_main_menu_join_pressed(ip: String) -> void:
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby._on_menu_control_lobby_started(lobby_interface, ip)

func _return_from_lobby_to_menu() -> void:
	if is_instance_valid(lobby_interface):
		lobby_interface.queue_free()
	main_menu.show()

enum EndLobbyCause{ABRUPT, KICKED, ORDERLY}

func _on_lobby_removed_from_lobby(cause: EndLobbyCause) -> void:
	_return_from_lobby_to_menu()
# for joiner
