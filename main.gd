extends Node2D

var peer = ENetMultiplayerPeer.new()
@export var player_scene: PackedScene


func _on_host_pressed() -> void:
	peer.create_server(135)
	multiplayer.multiplayer_peer = peer
	multiplayer.peer_connected.connect(_add_player)
	_add_player()

func _on_join_pressed() -> void:
	peer.create_client("localhost", 135)
	multiplayer.multiplayer_peer = peer

# 1 = host
func _add_player(id: int = 1) -> void:
	
	var player = player_scene.instantiate()
	player.name = str(id)
	
	#el error está en esto
	call_deferred("add_child", player)
	
	
