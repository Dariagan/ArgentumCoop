extends Control

signal host_pressed
signal join_pressed
func _on_quick_start_pressed() -> void:
	pass 

func _on_host_pressed() -> void:
	hide()
	host_pressed.emit()
	
func _on_join_pressed() -> void:
	hide()
	join_pressed.emit()
	


# Replace with function body.



func _on_line_edit_text_submitted(new_text: String) -> void:
	pass # Replace with function body.
