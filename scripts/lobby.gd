extends Node
# local peer
var peer = ENetMultiplayerPeer.new()
# local username
var _username: String 

@onready var world: Node2D = $"../GameWorld"
@export var character_creation_scene: PackedScene

var _lobby_interface: LobbyInterface

var _players: Array = []
var _peers: Array = []
var _ready_peers: Array = []

signal removed_from_lobby(kicked: bool)

func _host() -> void:
	peer.create_server(135)
	multiplayer.multiplayer_peer = peer
	multiplayer.peer_connected.connect(_on_player_join)
	multiplayer.peer_disconnected.connect(_on_player_disconnect)
	_players.push_back(_username)
	_peers.push_back(1)

func _join(ip: String) -> void:
	peer.create_client(ip, 135)
	multiplayer.multiplayer_peer = peer
	
# executed ONLY server-side when a player joins
func _on_player_join(peer_id: int) -> void:
	await get_tree().create_timer(0.5).timeout
	_lobby_interface._update_lobby_title_for_client(peer_id)
	_request_player_username(peer_id)
	_players.push_back(await player_username_received)
	_peers.push_back(peer_id)
	_sync_state_for_clients()
	
	# for host:
	_update_players_for_gui()
	
func _sync_state_for_clients() -> void: 
	#each is executed on every client's machine
	_give_player_list.rpc(_players) 
	_give_peer_list.rpc(_peers)
	_give_ready_peers_list.rpc(_ready_peers)
	
@rpc func _give_player_list(players: Array) -> void: _players = players; _update_players_for_gui()
@rpc func _give_peer_list(peers: Array) -> void: _peers = peers
@rpc func _give_ready_peers_list(ready_peers: Array) -> void: _ready_peers = ready_peers	
	
func _cancel_host() -> void:
	clear_arrays()
	_remove_from_lobby.rpc(false)
	await get_tree().create_timer(1).timeout
	peer.close()
	peer = ENetMultiplayerPeer.new()
		
func _leave_as_client() -> void:
	peer.close()
	clear_arrays()
		
func clear_arrays() -> void:
	_players.clear()
	_peers.clear()
	_ready_peers.clear()
		
func _on_player_disconnect(peer_id: int) -> void:
	_players.remove_at(_peers.find(peer_id))
	_peers.erase(peer_id)
	_ready_peers.erase(peer_id)
	_sync_state_for_clients()
	
	_update_players_for_gui()
				
@rpc
func _remove_from_lobby(kicked: bool) -> void:
	_players.clear()
	_peers.clear()
	_ready_peers.clear()
	removed_from_lobby.emit(kicked)

func _update_players_for_gui() -> void:
	if is_instance_valid(_lobby_interface):
		_lobby_interface.update_player_list(_players)
	
# when ready is pressed in the GUI
func _on_player_ready(ready: bool) -> void:
	if multiplayer.get_unique_id() != 1:
		_peer_is_ready.rpc(ready)
		
	elif _is_everybody_ready():
		print("starting")

@rpc("call_local", "any_peer")
func _peer_is_ready(ready: bool) -> void:
	var peer: int = multiplayer.get_remote_sender_id()
	if ready and peer not in _ready_peers:
		_ready_peers.push_back(peer)
	elif not ready:
		_ready_peers.erase(peer)
	
func _is_everybody_ready() -> bool:
	return _ready_peers.size() + 1 == _peers.size()
		
	
# EN VEZ DE TODO ESTO HACER Q APENAS SE UNA EL PLAYER ESTE EJECUTE UN .RPC_ID(1, _username) Y EL SERVER SE LO QUEDA AHÍ
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


func _on_menu_control_lobby_started(lobby_interface: LobbyInterface, joined_ip: String) -> void:
	_lobby_interface = lobby_interface
	_lobby_interface.update_username(_username)
	_lobby_interface.ready_toggled.connect(_on_player_ready)
	if not joined_ip:
		_lobby_interface.set_up_host_lobby(_username)
		_lobby_interface.player_clicked_leave.connect(_cancel_host, CONNECT_ONE_SHOT)
		_host()
	else:
		_lobby_interface.set_up_joiner_lobby()
		_lobby_interface.player_clicked_leave.connect(_leave_as_client, CONNECT_ONE_SHOT)
		_join(joined_ip)

# used by main.gd
func update_username(username: String):
	_username = username
