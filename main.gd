extends Node2D

# MUY IMPORTANTE: SI SE COLOCA CUALQUIER COSA EN EL SCENE TREE, HAY QUE PONER UN MULTIPLAYER SPAWNER PARA ESTE

# CADA VEZ Q SE USA ADD_CHILD, ASEGURARSE Q TIENE UN SYNCHRONIZER PUESTO

var peer = ENetMultiplayerPeer.new()
@export var player_scene: PackedScene

@onready var world: Node2D = $GameWorld

var players: Array[Character]

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

# MUY IMPORTANTE: SI SE COLOCA CUALQUIER COSA EN EL SCENE TREE, HAY QUE PONER UN MULTIPLAYER SPAWNER PARA ESTE
