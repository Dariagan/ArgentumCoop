extends RefCounted
class_name AiProcess

var being: Being

func _init(_being: Being = null):
	assert(_being != null)
	being = _being


func behave(delta: float):
	
	if being.istate.master:
		goto_being(being.istate.master, delta, 0)
		pass
	pass

# posible optimizacion, solo calcular hasta la primera mitad de los puntos del camino (no pathear a un punto arbitrario intermedio, puede ser inválido)

var i: int = 0

var recalc_path_timeout: float
func goto_being(target_being: Being, delta: float, distance_to: float):
	i+=1
	#recalc_path_timeout -= target_being.acceleration/2500*delta/distance_to
	recalc_path_timeout -= delta
	if recalc_path_timeout < 0:
		being.nav.target_position = target_being.global_position
		recalc_path_timeout = 0.5
	
	var next_pos: Vector2 = being.nav.get_next_path_position()
	being._direction_axis = being.global_position.direction_to(next_pos)
