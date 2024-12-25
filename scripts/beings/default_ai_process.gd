extends RefCounted
class_name AiProcess #extender el script

var mmyself: Being

func _init(_being: Being = null):
	assert(_being != null)
	mmyself = _being

#overridear esto
func behave(delta: float):
	if mmyself.mistate.mmaster:
		goto_being(delta, mmyself.mistate.mmaster, mmyself.distance_to(mmyself.mistate.mmaster), 150, 10, 1, wander)
		pass
	pass
# posible optimizacion: solo calcular hasta la primera mitad de los puntos del camino (no pathear a un punto arbitrario intermedio, puede ser inválido)

var i: int = 0
var mcheck_within_range_remai_time: float = -1000
var recalc_path_remai_time: float; var chase: bool = false; 
# llamar con super.gotobeing() en subscripts (heredan de este)
func goto_being(delta:float, target:Being, curr_dist:float, maxdist:float=0, mcheck_within_range_tperiod:float=25, 
	path_update_time_mult:float=1, on_min_distance_reached: Callable=Callable()) -> void:
	if mcheck_within_range_remai_time == -1000: mcheck_within_range_remai_time = mcheck_within_range_tperiod
	if (mcheck_within_range_remai_time < 0 and curr_dist > maxdist):
		chase = true; mmyself.mcontroller_speed_multiplier = randf_range(0.05, 0.2)
	if target.velocity.length() > 150: 
		chase = true; mmyself.mcontroller_speed_multiplier = 1.0
	if chase:
		if curr_dist <= maxdist: 
			mcheck_within_range_remai_time = mcheck_within_range_tperiod;
			chase = false; mmyself.mcontroller_speed_multiplier = 0.1
			mpicked_dir = Utils.random_vector2_with_length(1)
			return;
		
		var own_velocity_factor: float = max(mmyself.velocity.length()*0.0035, 0.3)
		var target_velocity_factor: float = max(target.velocity.length()*0.0035, 0.3)
		var relative_velocity_factor: float = target_velocity_factor*own_velocity_factor
		var target_distance_factor: float = curr_dist/400
		var final_factor: float = relative_velocity_factor/(target_distance_factor*path_update_time_mult)
		
		recalc_path_remai_time -= delta * final_factor
		if recalc_path_remai_time < 0:
			mmyself.mnav.target_position = target.global_position
			recalc_path_remai_time = 0.5 #dejar hardcodeado
		
		var next_pos: Vector2 = mmyself.mnav.get_next_path_position()
		
		mmyself.mdirection_axis = mmyself.global_position.direction_to(next_pos)
	else: #within range
		mcheck_within_range_remai_time -= delta * curr_dist/maxdist
		if on_min_distance_reached.is_valid():
			on_min_distance_reached.call(delta)
	i+=1
var mrem_time: float = -1.0; var mstay: bool = true
var mpicked_dir: Vector2
func wander(delta: float):
	mrem_time -= delta
	if mrem_time < 0: 
		mstay = randi() % 2
		mrem_time = randf_range(2.0, 10.0)
		if not mstay:
			mpicked_dir=Utils.random_vector2_with_length(1)
			mmyself.mcontroller_speed_multiplier = randf_range(0.05, 0.3)
		
	if not mstay:
		mmyself.mdirection_axis = mpicked_dir
	else: 
		mmyself.mdirection_axis = Vector2.ZERO
	
	
	
