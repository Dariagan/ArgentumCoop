extends Node

var peer = ENetMultiplayerPeer.new()
#@export var player_scene: PackedScene

var _username: String 

@onready var world: Node2D = $"../GameWorld"

var _players: Array = []

signal player_list_updated(players: Array)

signal player_username_received(username: String)

func _host() -> void:
	peer.create_server(135)
	multiplayer.multiplayer_peer = peer
	multiplayer.peer_connected.connect(_on_player_join)
	_players.push_back(_username)

func _join() -> void:
	peer.create_client("localhost", 135)
	multiplayer.multiplayer_peer = peer
	
# executed ONLY server-side when a player joins
func _on_player_join(peer_id: int) -> void:
	await get_tree().create_timer(0.5).timeout
	_request_player_username(peer_id)
	_players.push_back(await player_username_received)
	player_list_updated.emit(_players)
	_give_player_list_to_joiner.rpc_id(peer_id, _players) #executed on the newly joined player's machine
	
	
	
@rpc
func _give_player_list_to_joiner(players: Array) -> void:
	_players = players
	player_list_updated.emit(_players)
	
# todos lo pueden llamar? arreglar!
# gets executed both in the function caller's PC, and in all the remote connected PCs
@rpc("any_peer", "call_local")
func _add_connected_player_to_player_list(new_player_username: String) -> void:

	_players.push_back(new_player_username)
	player_list_updated.emit(_players)

func _add_player(id: int = 1) -> void:
	pass
	"""
	var player: Character = player_scene.instantiate()
	player.name = str(id)
	
	world.spawn_player(player)
	world.visible = true"""

var requested_peer: int = -1
func _request_player_username(peer_id: int) -> void:
	requested_peer = peer_id
	_return_player_username.rpc_id(peer_id)
@rpc
func _return_player_username() -> void:
	_receive_player_username.rpc_id(multiplayer.get_remote_sender_id(), _username)
@rpc("any_peer")
func _receive_player_username(username: String) -> void:
	if requested_peer == multiplayer.get_remote_sender_id():
		player_username_received.emit(username)
	requested_peer = -1

func _on_menu_container_hosting() -> void:
	_host()

func _on_menu_container_joining() -> void:
	_join()

func update_username(username: String):
	_username = username
	
	
