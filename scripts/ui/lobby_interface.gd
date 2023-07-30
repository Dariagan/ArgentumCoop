extends PanelContainer

@onready var h_box_container_2:HBoxContainer=$VBoxContainer/HBoxContainer2

var start_button: Button

var player_id: String

signal is_ready(hosting: bool)
signal host_pressed_start
signal ready_toggled(value: bool)

func _on_main_menu_host_pressed() -> void:
	_add_start_button(true)
	show()
	is_ready.emit(true)


func _on_main_menu_join_pressed() -> void:
	_add_start_button(false)
	show()
	is_ready.emit(false)
	
func _add_start_button(hosting: bool) -> void:
	
	if hosting:
		start_button = Button.new()
		start_button.text = "Start"
		start_button.connect("pressed", _host_start_button_pressed)
	else:
		start_button = CheckBox.new()
		start_button.text = "Ready"
		start_button.connect("toggled", _ready_button_pressed)
	
	start_button.custom_minimum_size.x = 300
	h_box_container_2.add_child(start_button)
	
	
func _host_start_button_pressed() -> void:
	host_pressed_start.emit()

func _ready_button_pressed(toggled: bool) -> void:
	ready_toggled.emit(toggled)
