extends Node

@onready var main_menu: Container = $MainMenu

var _username: String

@export var lobby_interface_scene: PackedScene
var lobby_interface: LobbyInterface

signal hosting(value: bool)

signal joining(value: bool)

signal player_ready(ready: bool)

func _ready() -> void:
	pass
	
func update_username(new_username: String):
	_username = new_username

func _on_lobby_player_list_updated(players: Array) -> void:
	if is_instance_valid(lobby_interface):
		lobby_interface.update_player_list(players)
		
func _on_lobby_interface_ready_pressed(value: bool):
	player_ready.emit(value)

# for host
func _on_main_menu_host_pressed() -> void:
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby_interface.update_username(_username)
	lobby_interface.set_up_host_lobby(_username)
	hosting.emit(true)
	lobby_interface.left.connect(cancel_host, CONNECT_ONE_SHOT)
	lobby_interface.ready_toggled.connect(_on_lobby_interface_ready_pressed)
	
func cancel_host():
	hosting.emit(false)
	_return_from_lobby_to_menu()
	
func _on_lobby_peer_joined(peer_id) -> void:
	lobby_interface._update_lobby_title_for_client(peer_id)
# for host

# for joiner
func _on_main_menu_join_pressed() -> void:
	joining.emit(true)
	lobby_interface = lobby_interface_scene.instantiate()
	add_child(lobby_interface)
	lobby_interface.update_username(_username)
	lobby_interface.set_up_joiner_lobby()
	lobby_interface.left.connect(leave_lobby, CONNECT_ONE_SHOT)
	lobby_interface.ready_toggled.connect(_on_lobby_interface_ready_pressed)
	
func leave_lobby():
	joining.emit(false)
	_return_from_lobby_to_menu()

func _return_from_lobby_to_menu() -> void:
	print(1)
	lobby_interface.queue_free()
	main_menu.show()
	
func _on_lobby_removed_from_lobby(kicked: bool) -> void:
	_return_from_lobby_to_menu()
# for joiner


