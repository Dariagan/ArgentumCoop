extends RefCounted
class_name AiProcess #extender el script

var mybeing: Being

func _init(_being: Being = null):
	assert(_being != null)
	mybeing = _being


#overridear esto
func behave(delta: float):
	if mybeing.istate.master:
		goto_being(delta, mybeing.istate.master, mybeing.distance_to(mybeing.istate.master), 150, 2)
		pass
	pass

# posible optimizacion, solo calcular hasta la primera mitad de los puntos del camino (no pathear a un punto arbitrario intermedio, puede ser inválido)

var i: int = 0
var recalc_path_remai_time: float # llamar con super.gotobeing() en subscripts
func goto_being(delta: float, target_being: Being, distance: float, min_distance: float = 0, path_update_time_mult: float = 1, on_min_distance_reached: Callable = Callable()):
	i+=1
	if distance > min_distance:
		var own_velocity_factor: float = max(mybeing.velocity.length()*0.0035, 0.3)
		var target_velocity_factor: float = max(target_being.velocity.length()*0.0035, 0.3)
		var relative_velocity_factor: float = target_velocity_factor*own_velocity_factor
		var target_distance_factor: float = distance/400
		var final_factor: float = relative_velocity_factor/(target_distance_factor*path_update_time_mult)
		
		if i%40==0:
			pass#print(mybeing.velocity.length()*0.003)
		recalc_path_remai_time -= delta * final_factor
		if recalc_path_remai_time < 0:
			mybeing.nav.target_position = target_being.global_position
			recalc_path_remai_time = 0.5 #dejar hardcodeado
		
		var next_pos: Vector2 = mybeing.nav.get_next_path_position()
		mybeing._direction_axis = mybeing.global_position.direction_to(next_pos)
	else:
		mybeing._direction_axis = Vector2.ZERO
		if on_min_distance_reached.is_valid():
			on_min_distance_reached.call_deferred()

var _nodir_rem_time: float = 1.0; var _maintaindir_rem_time: float
func wander(delta: float, _nodir_restime: float, _maintaindir_restime: float):
	_nodir_rem_time -= delta
	_maintaindir_rem_time
	
	
	
