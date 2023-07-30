extends PanelContainer

@onready var v_box_container: VBoxContainer = $VBoxContainer
@onready var h_box_container_2:HBoxContainer=$VBoxContainer/HBoxContainer2
@onready var players_label: Label = $VBoxContainer/HBoxContainer/PlayersLabel

var _player_id: String

var lobby_title_line_edit: LineEdit 
var lobby_title_label: Label 

var start_button: Button

signal is_ready(hosting: bool)
signal host_pressed_start
signal ready_toggled(value: bool)

func set_up_host_lobby(host_name: String) -> void:
	_add_start_button(true)
	_add_lobby_title_line_edit(host_name)
	players_label.text += "%s (host)\n" % host_name
	show()
	is_ready.emit(true)
	
func _add_lobby_title_line_edit(host_name: String) -> void:
	lobby_title_line_edit = LineEdit.new()
	lobby_title_line_edit.text = "%s's lobby" % host_name	
	lobby_title_line_edit.max_length = 70
	lobby_title_line_edit.custom_minimum_size.y = 50
	lobby_title_line_edit.alignment = HORIZONTAL_ALIGNMENT_CENTER
	lobby_title_line_edit.caret_blink = true
	v_box_container.add_child(lobby_title_line_edit)
	v_box_container.move_child(lobby_title_line_edit, 0)
	lobby_title_line_edit.connect("text_submitted", _on_lobby_title_text_submitted)
	
func _add_lobby_title_label(host_name: String) -> void:
	lobby_title_label = Label.new()
	lobby_title_label.text = "%s's lobby " % host_name	
	lobby_title_label.max_length = 70
	lobby_title_label.custom_minimum_size.y = 50
	lobby_title_label.alignment = HORIZONTAL_ALIGNMENT_CENTER
	v_box_container.add_child(lobby_title_label)
	v_box_container.move_child(lobby_title_label, 0)
	

func set_up_joiner_lobby() -> void:
	_add_start_button(false)
	show()
	is_ready.emit(false)
	
func _add_start_button(hosting: bool) -> void:
	
	if hosting:
		start_button = Button.new()
		start_button.text = "Start"
		start_button.connect("pressed", _start_button_pressed)
	else:
		start_button = CheckBox.new()
		start_button.text = "Ready"
		start_button.connect("toggled", _ready_button_pressed)
	
	start_button.custom_minimum_size.x = 300
	h_box_container_2.add_child(start_button)
	
	
func _start_button_pressed() -> void:
	host_pressed_start.emit()

func _ready_button_pressed(toggled: bool) -> void:
	ready_toggled.emit(toggled)

func _on_lobby_title_text_submitted(new_text: String) -> void:
	_update_lobby_title.rpc(new_text)
@rpc("call_local")
func _update_lobby_title(new_text: String):
	if lobby_title_label:
		lobby_title_label.text = new_text


func update_player_id(new_player_id: String):
	_player_id = new_player_id
	

# hacer lo de abajo un component reusable

@onready var write_msg_box: TextEdit = $VBoxContainer/HBoxContainer2/WriteMsgBox
@onready var chat_label: Label = $VBoxContainer/HBoxContainer/ScrollContainer/ChatLabel
@onready var scroll_container: ScrollContainer = $VBoxContainer/HBoxContainer/ScrollContainer

const MESSAGE_LIMIT = 144

var current_text = ''
var cursor_line = 0
var cursor_column = 0	

func _ready() -> void:
	# Connect the `_gui_input` event to the current node (PanelContainer)
	write_msg_box.connect("gui_input", _on_write_msg_box_gui_input)

func _on_write_msg_box_gui_input(event):
	if event is InputEventKey:
		var key_event = event as InputEventKey

		if key_event.keycode == KEY_ENTER and write_msg_box.text == "\n":
			write_msg_box.set_caret_line(0)
		else:
			add_chat_message()
		
			
func add_chat_message() -> void:
	var new_text : String = write_msg_box.text.strip_edges() 
	if new_text.length() > 0:
		chat_label.text += "%s: %s\n" % [_player_id, new_text]
		write_msg_box.text = ""
		write_msg_box.set_caret_line(0)
		write_msg_box.set_caret_column(0)
		scroll_container.scroll_vertical = scroll_container.get_v_scroll_bar().max_value

func _on_write_msg_box_text_changed() -> void:
	var new_text : String = write_msg_box.text
	if new_text.length() > MESSAGE_LIMIT:
		write_msg_box.text = current_text
		# when replacing the text, the cursor will get moved to the beginning of the
		# text, so move it back to where it was 
		write_msg_box.set_caret_line(cursor_line)
		write_msg_box.set_caret_column(cursor_column)

	current_text = write_msg_box.text
	# save current position of cursor for when we have reached the limit
	cursor_line = write_msg_box.get_caret_line()
	cursor_column =write_msg_box.get_caret_column()
