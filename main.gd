extends Node2D

var peer = ENetMultiplayerPeer.new()
@export var player_scene: PackedScene

@onready var world: GameWorld = $GameWorld


func _on_quick_start_pressed() -> void:
	pass 

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
	
	var player: Character = player_scene.instantiate()
	player.name = str(id)
	
	world.spawn_player(player)
	world.visible = true

