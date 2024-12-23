class_name Utils


static func random_vector2_with_length(length: float) -> Vector2:
	var angle = randf() * TAU  
	var direction = Vector2(cos(angle), sin(angle))
	return direction * length
