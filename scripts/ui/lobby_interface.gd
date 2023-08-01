extends PanelContainer
class_name LobbyInterface


@onready var players_label: Label = $VBoxContainer/HBoxContainer2/PlayersLabel
@onready var chat_component: Node = $ChatComponent
@onready var h_box_container: HBoxContainer = $VBoxContainer/HBoxContainer1

@onready var h_box_container_3: HBoxContainer = $VBoxContainer/HBoxContainer3

var _username: String

var lobby_title_line_edit: LineEdit 
var lobby_title_label: Label 

var start_button: Button

signal ready_toggled(value: bool)

signal left

func update_player_list(new_player_list: Array) -> void:
	players_label.text = ""
	for i in range(0, new_player_list.size()):
		if i == 0:
			players_label.text += "%s (host)\n" % new_player_list[i]
		else:
			players_label.text += new_player_list[i] + "\n"

func set_up_host_lobby(host_name: String) -> void:
	_add_start_button(true)
	_add_lobby_title_line_edit(host_name)
	players_label.text += "%s (host)\n" % host_name
	
	
func _add_lobby_title_line_edit(host_name: String) -> void:
	lobby_title_line_edit = LineEdit.new()
	lobby_title_line_edit.text = "%s's lobby" % host_name	
	lobby_title_line_edit.max_length = 70
	lobby_title_line_edit.alignment = HORIZONTAL_ALIGNMENT_CENTER
	lobby_title_line_edit.size_flags_horizontal =Control.SIZE_EXPAND_FILL
	lobby_title_line_edit.caret_blink = true
	h_box_container.add_child(lobby_title_line_edit)
	h_box_container.move_child(lobby_title_line_edit, 0)
	lobby_title_line_edit.connect("text_submitted", _on_lobby_title_text_submitted)
	
	
func _add_lobby_title_label(new_title: String) -> void:
	lobby_title_label = Label.new()
	lobby_title_label.text = new_title
	lobby_title_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	lobby_title_label.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	h_box_container.add_child(lobby_title_label)
	h_box_container.move_child(lobby_title_label, 0)
	

func set_up_joiner_lobby() -> void:
	_add_start_button(false)
	_add_lobby_title_label("")
	
	
	
func _add_start_button(hosting: bool) -> void:
	if hosting:
		start_button = Button.new()
		start_button.text = "Start"
		start_button.connect("pressed", _ready_button_pressed)
	else:
		start_button = CheckBox.new()
		start_button.text = "Ready"
		start_button.connect("toggled", _ready_button_pressed)
	
	start_button.custom_minimum_size.x = 300
	h_box_container_3.add_child(start_button)
	


func _ready_button_pressed(toggled: bool = true) -> void:
	ready_toggled.emit(toggled)
	
func _update_lobby_title_for_client(peer_id: int):
	_update_lobby_title_client_side.rpc_id(peer_id, lobby_title_line_edit.text)

func _on_lobby_title_text_submitted(new_text: String) -> void:
	_update_lobby_title_client_side.rpc(new_text)
	
@rpc
func _update_lobby_title_client_side(new_text: String):
	if lobby_title_label:
		lobby_title_label.text = new_text

func update_username(new_username: String):
	_username = new_username
	chat_component.username = _username	

func _on_leave_button_pressed() -> void:
	left.emit()
