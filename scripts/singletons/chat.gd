extends AudioStreamPlayer

var chat_text_edit: TextEdit: set = set_text_edit
var chat_label: Label : set = set_chat_label

func set_chat_label(label: Label):
	chat_label = label
	chat_label.text = chat_log
	_scroll_container = label.get_parent()
	
func set_text_edit(text_edit: TextEdit):
	chat_text_edit = text_edit
	chat_text_edit.gui_input.connect(_on_input_event)
	chat_text_edit.text_changed.connect(_on_write_msg_box_text_changed)

var message_char_limit: int = 144
var input_enabled: bool = false
var taunts_enabled: bool = true
var chat_log: String

var _scroll_container: ScrollContainer

var audio_stream_poly_playback: AudioStreamPlaybackPolyphonic
func _ready() -> void:
	
	var audio_stream_poly: AudioStreamPolyphonic = AudioStreamPolyphonic.new()
	audio_stream_poly.polyphony = 10
	
	self.stream = audio_stream_poly
	self.play()
	audio_stream_poly_playback = self.get_stream_playback()

func _on_input_event(event: InputEvent):
	if input_enabled and event is InputEventKey and event.pressed and not event.echo:
		if event.keycode == KEY_ENTER:
			get_viewport().set_input_as_handled()
			add_chat_message.rpc(chat_text_edit.text.strip_edges(), Global.username)
			chat_text_edit.text = ""
			chat_text_edit.set_caret_line(0)
			chat_text_edit.set_caret_column(0)

var audio_timeout: float
func _process(delta):
	audio_timeout -= delta
	audio_timeout = max(audio_timeout, -0.1) 
			
@rpc("any_peer", "call_local")
func add_chat_message(new_text : String, sender: String) -> void:

	if new_text.length() > 0:
		if taunts_enabled and audio_timeout < 0 and new_text.is_valid_int() and new_text.length() <= 3:
			var taunt_number : int = new_text.to_int()
			if taunt_number >= 0 and taunt_number < Global.taunt_sounds.values().size():
				audio_stream_poly_playback.play_stream(Global.taunt_sounds.values()[taunt_number])
				audio_timeout = 0.5
		chat_label.text += "%s: %s\n" % [sender, new_text]
		chat_log = chat_label.text
		await get_tree().create_timer(0.001).timeout
		_scroll_container.scroll_vertical = _scroll_container.get_v_scroll_bar().max_value as float

var prev_textedit_text = ''
var prev_cursor_line = 0
var prev_cursor_column = 0	
func _on_write_msg_box_text_changed() -> void:
	var new_text : String = chat_text_edit.text
	if new_text.length() > message_char_limit:
		chat_text_edit.text = prev_textedit_text
		chat_text_edit.set_caret_line(prev_cursor_line)
		chat_text_edit.set_caret_column(prev_cursor_column)
	
	prev_textedit_text = chat_text_edit.text
	prev_cursor_line = chat_text_edit.get_caret_line()
	prev_cursor_column = chat_text_edit.get_caret_column()
