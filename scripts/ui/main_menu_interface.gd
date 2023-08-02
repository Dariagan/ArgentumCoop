extends Control

signal host_pressed
signal join_pressed
signal name_changed(new_name: String)

func _on_quick_start_pressed() -> void:
	pass 

func _on_host_pressed() -> void:
	hide()
	host_pressed.emit()
	
func _on_join_pressed() -> void:
	hide()
	join_pressed.emit()
	
func _on_line_edit_text_submitted(new_text: String) -> void:
	name_changed.emit(new_text)
