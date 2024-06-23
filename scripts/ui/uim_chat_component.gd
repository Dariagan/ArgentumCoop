extends Node

@export var chat_text_edit: TextEdit
@export var chat_label: Label
@export var scroll_container: ScrollContainer 
@export var MESSAGE_CHAR_LIMIT: int = 144

var input_enabled: bool = true


var current_text = ''
var cursor_line = 0
var cursor_column = 0	

func _ready() -> void:
	chat_text_edit.gui_input.connect(_on_input_event)
	


func _on_input_event(event: InputEvent):
	if event is InputEventKey and input_enabled and event.keycode == KEY_ENTER\
		and event.pressed and not event.echo:
		get_viewport().set_input_as_handled()
		add_chat_message.rpc(chat_text_edit.text.strip_edges(), GlobalData.username)
			
@rpc("any_peer", "call_local")
func add_chat_message(new_text : String, sender: String) -> void:

	if new_text.length() > 0:
		chat_label.text += "%s: %s\n" % [sender, new_text]
		chat_text_edit.text = ""
		chat_text_edit.set_caret_line(0)
		chat_text_edit.set_caret_column(0)
		
		await get_tree().create_timer(0.001).timeout
		scroll_container.scroll_vertical = scroll_container.get_v_scroll_bar().max_value as float

func _on_write_msg_box_text_changed() -> void:
	var new_text : String = chat_text_edit.text
	if new_text.length() > MESSAGE_CHAR_LIMIT:
		chat_text_edit.text = current_text
		chat_text_edit.set_caret_line(cursor_line)
		chat_text_edit.set_caret_column(cursor_column)

	current_text = chat_text_edit.text
	cursor_line = chat_text_edit.get_caret_line()
	cursor_column = chat_text_edit.get_caret_column()
