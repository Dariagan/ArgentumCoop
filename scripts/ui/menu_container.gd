extends Container

@onready var main_menu: Container = $MainMenu
@onready var lobby_interface: PanelContainer = $LobbyInterface
var _player_id: String

func _ready() -> void:
	pass
	
func update_player_id(new_player_id: String):
	_player_id = new_player_id
	lobby_interface.update_player_id(_player_id)
	

func _on_main_menu_host_pressed() -> void:
	lobby_interface.set_up_host_lobby(_player_id)
