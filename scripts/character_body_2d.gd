extends CharacterBody2D

const speed = 100

func _physics_process(delta: float) -> void:
	player_movement(delta)

func player_movement(delta):
	
	var input_vector = Vector2.ZERO
	input_vector.x = Input.get_action_strength("ui_right") - Input.get_action_strength("ui_left")
	
	
	move_and_slide()
