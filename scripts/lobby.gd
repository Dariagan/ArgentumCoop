extends Node
var mp_peer = ENetMultiplayerPeer.new()

var mlobby_interface: LobbyInterface

var mplayers: PackedStringArray = [] # se tiene q pushear en el momento q hostea porque puede cambiar, no se puede poner desde el inicio de todo
var mpeers: PackedInt32Array = [1] #hay que usar esto en vez de multiplayer get_peer porque si se va alguien se pierde toda la ordenacion por indice i entre los distintos arrays
var mready_peers: PackedInt32Array = [] # doesn't include host
var mcharacters_spawn_data: Array = [{}] 
const PORT: int = 1025 #WARNING PORTS BELOW 1024 MAY NOT WORK

func _ready():
	if Config.insta_start: start_new_game()
	multiplayer.connected_to_server.connect(_clientfn_on_connect)
	multiplayer.connection_failed.connect(_clientfn_on_fail_connect)#todo retornar si falla
	multiplayer.server_disconnected.connect(_reset_arrays)
	multiplayer.server_disconnected.connect(%MenuControl._return_from_lobby_to_menu)

func _clientfn_on_peer_closed() -> void:
	multiplayer.server_disconnected.disconnect(_clientfn_on_peer_closed)
	if not morderly_shutdown:
		pass#todo abrupt closing message

func _clientfn_on_connect(): _clientfn_send_username_to_host.rpc_id(1, Global.username)
func _clientfn_on_fail_connect(): push_error("failed to connect to %s:%d" % [mjoined_ip, PORT])

var mjoined_ip: String

func _on_join_pressed():
	pass

func _on_menu_control_lobby_started(lobby_interface: LobbyInterface, joined_ip: String) -> void:
	mlobby_interface = lobby_interface; mjoined_ip = joined_ip
	_connect_signals(mlobby_interface)
	#mlobby_interface.follower_body_i_selected.connect(_on_follower_body_selected)
	
	var is_host: bool = not joined_ip;
	if is_host:
		mlobby_interface.set_up_host_lobby(Global.username)
		multiplayer.peer_connected.connect(_hostfn_on_player_join)
		multiplayer.peer_disconnected.connect(_hostfn_on_player_disconnect)
		multiplayer.server_disconnected.connect(_hostfn_disconnect_signals)
		_hostfn_create_server()
	else:#client
		multiplayer.server_disconnected.connect(_clientfn_on_peer_closed)
		mlobby_interface.set_up_joiner_lobby()
		_clientfn_create_client(joined_ip)

func _hostfn_create_server() -> void:
	mplayers.push_back(Global.username); mp_peer.create_server(PORT)
	multiplayer.multiplayer_peer = mp_peer
	
func _clientfn_create_client(ip: String): mp_peer.create_client(ip, PORT); multiplayer.multiplayer_peer=mp_peer
	
func _hostfn_on_player_join(peer_id: int) -> void:
	if peer_id != 1:
		await _username_received#TODO poner timeout
		mlobby_interface._update_lobby_title_for_client(peer_id)
		mpeers.push_back(peer_id)
		mcharacters_spawn_data.push_back({})
		_clientfn_get_arrays.rpc(mpeers, mplayers, mready_peers, mcharacters_spawn_data)
		_update_players_for_gui()
func _hostfn_on_player_disconnect(peer_id: int) -> void:
	if peer_id != 1:
		var ready_peer_i: int = mready_peers.find(peer_id)
		if ready_peer_i != -1:
			mready_peers.remove_at(ready_peer_i)
		var peer_i: int = mpeers.find(peer_id)
		mplayers.remove_at(peer_i)
		mcharacters_spawn_data.remove_at(peer_i)
		mpeers.remove_at(peer_i)
		_clientfn_get_arrays.rpc(mpeers, mplayers, mready_peers, mcharacters_spawn_data)
		_update_players_for_gui()
@rpc
func _clientfn_get_arrays(peers: PackedInt32Array, players: PackedStringArray, 
		ready_peers: PackedInt32Array, characters_spawn_data: Array) -> void:
	mplayers = players; _update_players_for_gui()
	mpeers = peers
	mready_peers = ready_peers
	mcharacters_spawn_data = characters_spawn_data
	
var morderly_shutdown = false
@rpc func _orderly_closed_lobby(): morderly_shutdown = true
	
func _hostfn_disconnect_signals() -> void:
	multiplayer.peer_connected.disconnect(_hostfn_on_player_join)
	multiplayer.peer_disconnected.disconnect(_hostfn_on_player_disconnect)
		
func _reset_arrays() -> void:
	morderly_shutdown = false; mpeers = [1]; mready_peers = [{}]
	mplayers.clear(); mcharacters_spawn_data.clear()
@rpc
func _clientfn_kick_from_lobby() -> void:
	morderly_shutdown = true#ta mal porq pondria el mismo mensaje q si se cerra el lobby y no hay forma de distinguirlos con solo este bool
	multiplayer.multiplayer_peer.close()#ejecuta señal on server closed

func _update_players_for_gui() -> void:
	if is_instance_valid(mlobby_interface): mlobby_interface.update_player_list(mplayers)
	
# when ready is pressed in the GUI
func _on_player_ready(pready: bool) -> void:
	if multiplayer.get_unique_id() != 1:
		_peer_is_ready.rpc(pready)
	elif _is_everybody_ready() or Config.ignore_joiners_readiness_on_start:
		_on_game_start.rpc()
		start_new_game()
		
@rpc("call_local")
func _on_game_start(): mlobby_interface.queue_free()

@rpc("call_local", "any_peer")
func _peer_is_ready(sender_is_ready: bool) -> void:
	var peer_id: int = multiplayer.get_remote_sender_id()
	if sender_is_ready and peer_id not in mready_peers:
		mready_peers.push_back(peer_id)
	elif not sender_is_ready:
		mready_peers.remove_at(mready_peers.find(peer_id))
#doesn't count host
func _is_everybody_ready() -> bool: return mready_peers.size() == multiplayer.get_peers().size()
			
func _connect_signals(lobby_interface: LobbyInterface):
	lobby_interface.clicked_leave.connect(mp_peer.close); lobby_interface.ready_toggled.connect(_on_player_ready)
	lobby_interface.name_changed.connect(_on_name_selected); lobby_interface.race_selected.connect(_on_race_selected)
	lobby_interface.sex_selected.connect(_on_sex_selected); lobby_interface.head_selected.connect(_on_head_selected)
	lobby_interface.class_selected.connect(_on_class_selected); lobby_interface.follower_selected.connect(_on_follower_selected)
	lobby_interface.body_scale_changed.connect(_on_body_scale_changed)
	
signal _username_received
@rpc("any_peer")
func _clientfn_send_username_to_host(username: String) -> void:
	if multiplayer.get_remote_sender_id() == multiplayer.get_peers()[-1]:#posible bug si se une uno justo
		mplayers.push_back(username); _username_received.emit()

#region Character creation synchronization
func _on_name_selected(new_name: String):
	if new_name: _allfn_update_characterization.rpc(Keys.NAME, new_name)
	else: _allfn_update_characterization.rpc(Keys.NAME)
func _on_race_selected(race: ControllableRace):
	if race: _allfn_update_characterization.rpc(Keys.RACE, race.mid)
	else: _allfn_update_characterization.rpc(Keys.RACE)
func _on_sex_selected(sex: Enu.Sex):
	if sex > 0: _allfn_update_characterization.rpc(Keys.SEX, sex)
	else: _allfn_update_characterization.rpc(Keys.SEX)
func _on_head_selected(head : SpriteData):
	if head: _allfn_update_characterization.rpc(Keys.HEAD, head.mid)
	else: _allfn_update_characterization.rpc(Keys.HEAD)
func _on_class_selected(klass: Klass):
	if klass: _allfn_update_characterization.rpc(Keys.KLASS, klass.mid)
	else: _allfn_update_characterization.rpc(Keys.KLASS)
func _on_follower_selected(follower: BeingGenTemplate):
	if follower: _allfn_update_characterization.rpc(Keys.FOLLOWERS, [follower.mid])
	else: _allfn_update_characterization.rpc(Keys.FOLLOWERS)
func _on_body_scale_changed(new_scale: Vector3):
	_allfn_update_characterization.rpc(Keys.BODY_SCALE, new_scale)
	
@rpc("call_local", "any_peer")
func _allfn_update_characterization(characterization_key: StringName, value = null): 
	var sender_i: int = mpeers.find(multiplayer.get_remote_sender_id())
	if value != null:
		mcharacters_spawn_data[sender_i][characterization_key] = value
	else:
		mcharacters_spawn_data[sender_i].erase(characterization_key)
#endregion

#region Pregame


func start_new_game() -> void:
	var tile_map: GdTileMap = $GdTileMap; tile_map.generate_world.rpc()
	var spawned_beings: Array[Being] = []
	var i: int = 0
	for player_start_data: Dictionary in mcharacters_spawn_data:
		if not player_start_data.has(Keys.NAME):
			player_start_data[Keys.NAME] = &"random"
		if not player_start_data.has(Keys.RACE):
			player_start_data[Keys.RACE] = &"controllable_random"
		if not player_start_data.has(Keys.KLASS):
			player_start_data[Keys.KLASS] = &"random"
		if not player_start_data.has(Keys.SEX):
			player_start_data[Keys.SEX] = &"random"
		if not player_start_data.has(Keys.HEAD):
			player_start_data[Keys.HEAD] = &"random"
		if not player_start_data.has(Keys.BODY):
			player_start_data[Keys.BODY] = &"random"
		
		player_start_data[Keys.FACTION] = Keys.PLAYER_FACTION_INSTANCE
		
		#extra health para los protagonists
		player_start_data[Keys.HEALTH_MULTIP] = 2	
		
		var player_being_preinit_data: BeingPreInit = BeingPreInit.construct(player_start_data)
		
		var being: Being = tile_map.spawn_starting_player(player_being_preinit_data, mpeers[i])
		spawned_beings.push_back(being)
		being.give_control.rpc(mpeers[i])
		i+=1
	
	MusicPlayer.play_playlist_shuffled(Keys.PEACE_ORDER, true)
	
#endregion
