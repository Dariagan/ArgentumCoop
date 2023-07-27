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

func _add_player(peer_id: int = 1) -> void:
	var player = player_scene.instantiate()
	
	player.name = str(peer_id)
	call_deferred("add_child", player)
	
	
