extends PanelContainer

@onready var h_box_container_1: HBoxContainer = $VBoxContainer/HBoxContainer1

@onready var h_box_container_3:HBoxContainer=$VBoxContainer/HBoxContainer3
@onready var players_label: Label = $VBoxContainer/HBoxContainer2/PlayersLabel
@onready var chat_component: Node = $ChatComponent
@onready var leave_button: Button = $VBoxContainer/HBoxContainer1/LeaveButton

var _player_id: String

var lobby_title_line_edit: LineEdit 
var lobby_title_label: Label 

var start_button: Button

signal is_ready(hosting: bool)
signal host_pressed_start
signal ready_toggled(value: bool)
signal left

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
	lobby_title_line_edit.size_flags_horizontal =Control.SIZE_EXPAND_FILL
	lobby_title_line_edit.caret_blink = true
	h_box_container_1.add_child(lobby_title_line_edit)
	h_box_container_1.move_child(lobby_title_line_edit, 0)
	lobby_title_line_edit.connect("text_submitted", _on_lobby_title_text_submitted)
	
func _add_lobby_title_label(host_name: String) -> void:
	lobby_title_label = Label.new()
	lobby_title_label.text = "%s's lobby " % host_name	
	lobby_title_label.max_length = 70
	lobby_title_label.custom_minimum_size.y = 50
	lobby_title_label.alignment = HORIZONTAL_ALIGNMENT_CENTER
	h_box_container_1.add_child(lobby_title_label)
	h_box_container_1.move_child(lobby_title_label, 0)
	

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
	h_box_container_3.add_child(start_button)
	
	
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
	chat_component.player_id = _player_id	

func _on_leave_button_pressed() -> void:
	left.emit()
