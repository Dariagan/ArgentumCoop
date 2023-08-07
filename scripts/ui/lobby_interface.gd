extends Node
class_name LobbyInterface

"""
TODO CHATROOM:
- VOLVER CADA PLAYER AJENO UN MENUDROPDOWN
- PONER BOTÓN "LOAD GAME" EN EL CHATROOM (HOST ONLY)
TODO CHARACTER CREATION:
- PONER BOTÓN "LOAD CHARACTER" ARRIBA A LA DERECHA
- PONER DROPDOWN "CHOOSE FOLLOWER BODY" ABAJO A LA IZQUIERDA
"""
@onready var players_label: Label = $ChatLobbyContainer/VBoxContainer/HBoxContainer2/PlayersLabel

@onready var chat_component: Node = $ChatLobbyContainer/VBoxContainer/ChatComponent

@onready var h_box_container: HBoxContainer = $ChatLobbyContainer/VBoxContainer/HBoxContainer1
@onready var h_box_container_3: HBoxContainer = $ChatLobbyContainer/VBoxContainer/HBoxContainer3


var _username: String

var lobby_title_line_edit: LineEdit 
var lobby_title_label: Label 

var start_button: Button

signal ready_toggled(value: bool)

signal player_clicked_leave

signal character_create(value: bool)

func _ready() -> void:
	get_children()[1].hide()

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
	lobby_title_line_edit.size_flags_horizontal = Control.SIZE_EXPAND_FILL
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
	
	start_button.custom_minimum_size.x = 150
	h_box_container_3.add_child(start_button)
	


func _ready_button_pressed(toggled: bool = true) -> void:
	ready_toggled.emit(toggled)
	
func _update_lobby_title_for_client(peer_id: int):
	_update_lobby_title_client_side.rpc_id(peer_id, lobby_title_line_edit.text)

func _on_lobby_title_text_submitted(new_text: String) -> void:
	_update_lobby_title_client_side.rpc(new_text)
	
@rpc
func _update_lobby_title_client_side(new_text: String) -> void:
	
	if lobby_title_label:
		lobby_title_label.text = new_text

func update_username(new_username: String) -> void:
	_username = new_username
	chat_component.username = _username	

func _on_leave_button_pressed() -> void:
	player_clicked_leave.emit()


func _on_create_character_button_pressed() -> void:
	get_children()[0].hide()
	chat_component.input_enabled = false
	get_children()[1].show()

# ------------------------------------------------------------
# -------------------- Character Creation --------------------
# ------------------------------------------------------------
func _on_lobby_button_pressed() -> void:
	get_children()[1].hide()
	chat_component.input_enabled = true
	get_children()[0].show()

signal name_changed(new_name: StringName)
signal race_selected(race: ControllableRace)
signal sex_selected(sex: Enums.Sex)
signal class_selected(klass: Class)
signal head_selected(index: int)
signal follower_selected(follower: UncontrollableRace)
#signal follower_body_selected

func _on_name_input_text_changed(new_name: String) -> void:
	name_changed.emit(new_name)
func _on_character_characterization_race_selected(race: ControllableRace) -> void:
	race_selected.emit(race)
	# mostrar stats en cuadradito
	
func _on_character_characterization_sex_selected(sex: Enums.Sex) -> void:
	sex_selected.emit(sex)
func _on_character_characterization_class_selected(klass: Class) -> void:
	class_selected.emit(klass)
	# mostrar stats en cuadradito
	
func _on_character_characterization_head_selected(index: int) -> void:
	head_selected.emit(index)
	# mostrar stats en cuadradito
	
func _on_character_characterization_follower_selected(follower: UncontrollableRace) -> void:
	follower_selected.emit(follower)
	# mostrar stats en cuadradito



