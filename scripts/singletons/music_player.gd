extends AudioStreamPlayer
# MusicPlayer
var continue_playing: bool = false
var thread: Thread

func _ready() -> void:
	self.finished.connect(semaphore.post)
	self.volume_db -= 10
	
@rpc("call_local")
func stop_prev_thread():
	if thread:
		continue_playing = false
		self.stop()
		self.finished.emit()
		thread.wait_to_finish()
		
var semaphore: Semaphore = Semaphore.new()

func play_playlist_shuffled(playlist_key: StringName, sync_mp: bool):
	if not Global.music.has(playlist_key):
		push_error("playlist %s not found" % playlist_key)
		return
	
	if not Global.music[playlist_key].is_empty():
		if sync_mp: stop_prev_thread.rpc()
		else: stop_prev_thread()
		
		semaphore = Semaphore.new()
		thread = Thread.new()
		continue_playing = true
		thread.start(_thread_play_playlist_shuffled.bind(playlist_key, sync_mp), Thread.PRIORITY_LOW)
	else:
		push_error("playlist %s is empty" % playlist_key)

func _thread_play_playlist_shuffled(playlist_key: StringName, sync_mp: bool):
	var playlist = Global.music[playlist_key]
	var remaining: Array[StringName] = []
	remaining.append_array(playlist.keys().duplicate())
	
	while continue_playing:
		var picked_track: StringName = remaining.pick_random()
		remaining.erase(picked_track)
		if sync_mp:
			play_stream_deferred_rpc.call_deferred(playlist_key, picked_track)
		else:
			_play_stream(playlist_key, picked_track)
		if continue_playing and remaining.is_empty():
			remaining.append_array(playlist.keys().duplicate())
		
		semaphore.wait()

func play_stream_deferred_rpc(playlist_key: StringName, soundtrack_id: StringName):
	_play_stream.rpc(playlist_key, soundtrack_id)

@rpc("call_local")
func _play_stream(playlist_key: StringName, soundtrack_id: StringName):
	self.stream = Global.music[playlist_key][soundtrack_id]
	self.play()
