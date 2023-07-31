extends Node
# local peer
var peer = ENetMultiplayerPeer.new()
# local username
var _username: String 
@onready var world: Node2D = $"../GameWorld"

var _players: Array = []
# used by server
var _peers: Array[int] = []

signal player_list_updated(players: Array)
signal player_joined(peer_id: int)

signal removed_from_lobby(kicked: bool)

func _host() -> void:
	peer.create_server(135)
	multiplayer.multiplayer_peer = peer
	multiplayer.peer_connected.connect(_on_player_join)
	multiplayer.peer_disconnected.connect(_on_player_disconnect)
	_players.push_back(_username)
	_peers.push_back(1)

func _join() -> void:
	peer.create_client("localhost", 135)
	multiplayer.multiplayer_peer = peer
# executed ONLY server-side when a player joins
func _on_player_join(peer_id: int) -> void:
	await get_tree().create_timer(0.5).timeout
	_request_player_username(peer_id)
	_players.push_back(await player_username_received)
	_peers.push_back(peer_id)
	player_list_updated.emit(_players)
	_give_player_list.rpc_id(peer_id, _players) #executed on the newly joined player's machine
	player_joined.emit(peer_id)
	
	
@rpc
func _give_player_list(players: Array) -> void:
	_players = players
	player_list_updated.emit(_players)
	

func _add_player(id: int = 1) -> void:
	pass
	"""
	var player: Character = player_scene.instantiate()
	player.name = str(id)
	
	world.spawn_player(player)
	world.visible = true"""



func _on_menu_container_hosting(hosting: bool) -> void:
	if hosting:
		_host()
	else: # cancelling lobby
		_players = []
		_peers = []
		_remove_from_lobby.rpc(false)
		await get_tree().create_timer(1).timeout
		peer.close()
		peer = ENetMultiplayerPeer.new()
		
func _on_menu_container_joining(joining: bool) -> void:
	if joining:
		_join()
	else: # leaving
		peer.close()
		_players = []
		
func _on_player_disconnect(peer_id: int):
	_players.remove_at(_peers.find(peer_id))
	_peers.erase(peer_id)
	_give_player_list.rpc(_players)
	player_list_updated.emit(_players)
				
@rpc
func _remove_from_lobby(kicked: bool):
	_players = []
	removed_from_lobby.emit(kicked)

# used by main.gd
func update_username(username: String):
	_username = username
	

# for requesting a peer's username
signal player_username_received(username: String)
var requested_peer: int = -1
func _request_player_username(peer_id: int) -> void:
	requested_peer = peer_id
	_return_player_username.rpc_id(peer_id)
@rpc
func _return_player_username() -> void:
	_receive_player_username.rpc_id(multiplayer.get_remote_sender_id(), _username)
@rpc("any_peer")
func _receive_player_username(username: String) -> void:
	if  multiplayer.get_remote_sender_id() == requested_peer:
		player_username_received.emit(username)
	requested_peer = -1
# for requesting a peer's username
