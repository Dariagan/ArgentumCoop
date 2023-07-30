extends Node


var peer = ENetMultiplayerPeer.new()
#@export var player_scene: PackedScene

@onready var world: Node2D = $"../GameWorld"

var players: Array[Character]

func _on_quick_start_pressed() -> void:
	pass 

func _host() -> void:

	peer.create_server(135)
	multiplayer.multiplayer_peer = peer
	multiplayer.peer_connected.connect(_add_player)
	_add_player()

func _join() -> void:
	peer.create_client("localhost", 135)
	multiplayer.multiplayer_peer = peer


# 1 = host
func _add_player(id: int = 1) -> void:
	pass
	"""
	var player: Character = player_scene.instantiate()
	player.name = str(id)
	
	world.spawn_player(player)
	world.visible = true"""


func _on_lobby_interface_is_ready(hosting: bool) -> void:
	if hosting:
		_host()
	else:
		_join()
	
	


func _on_main_menu_host_pressed() -> void:
	pass # Replace with function body.
