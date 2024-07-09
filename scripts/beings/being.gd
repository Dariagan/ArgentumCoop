extends CharacterBody2D

class_name Being
#para persistirlo habrá que usar packedscene para guardar las cosas custom children q puede ser q tenga, sino habrá q iterar por cada child

var uid: int

var acceleration = 2500
var ai_process: AiProcess = AiProcess.new(self)

var friction = 1600 #hacer q provenga de la tile en custom data
@onready var camera_2d: Camera2D = $Camera2D; @onready var body_holder: Node2D = $BodyHolder
@onready var istate: BeingInternalState = $InternalState
@onready var body: AnimatedBodyPortion = $BodyHolder/Body; @onready var head: AnimatedBodyPortion = $BodyHolder/Head
@onready var name_label = $NameLabel; @onready var nav = $NavigationAgent2D
@onready var tile_map: RustTileMap = get_parent()

signal load_tiles_around_me(coords: Vector2, chunk_size: Vector2i, uid: int)
#NO BORRAR
func _ready():
	load_tiles_around_me.connect(tile_map.load_tiles_around)

#constructs for multiplayer too
func construct(preiniter: BeingStatePreIniter, uid: int) -> void:
	if preiniter.sprite_body:
		body.construct(preiniter.sprite_body, preiniter.body_scale)
		if preiniter.sprite_head:
			head.construct(preiniter.sprite_head, preiniter.head_scale, preiniter.sprite_body.head_v_offset, preiniter.body_scale.z)
			
	istate.construct_from_seri.rpc(preiniter.istate.serialize())
	
	set_ai_process.rpc()
	
	var show_label: bool = istate.faction is PlayerFaction or (istate.being_gen_template and istate.being_gen_template.display_being_name)
	
	set_name_label_text_and_color.rpc(preiniter.name, istate.faction.color, show_label)
	
	self.setsync_node_name_and_uid.rpc(uid)
	#TODO key press para ocultar las namelabels de todos (usar el grupo)

@rpc("call_local")
func set_ai_process():
	if istate.being_gen_template and istate.being_gen_template.ai_process:
		ai_process = istate.being_gen_template.ai_process.new(self)
	elif istate.race.ai_process:
		ai_process = istate.race.ai_process.new(self)

@rpc("call_local") func set_name_label_text_and_color(text: String, color: Color, show_label: bool): 
	var ui_show_labels: bool = true
	
	name_label.text = text; name_label.visible = show_label and ui_show_labels
	name_label.label_settings = name_label.label_settings.duplicate(); name_label.label_settings.font_color = color; 

var controlling_peer: int = 0 : set = set_controlling_peer
func set_controlling_peer(peer: int): controlling_peer = max(0, peer); if controlling_peer == 0: wall_min_slide_angle = 0

@rpc("call_local", "any_peer")
func give_control(new_controller_peer: int) -> void:
	if (controlling_peer==0 or multiplayer.get_remote_sender_id() == controlling_peer)\
	   and istate.faction is PlayerFaction and istate.race is ControllableRace:
		set_multiplayer_authority(new_controller_peer)
		controlling_peer = new_controller_peer
		if new_controller_peer == multiplayer.get_unique_id():
			camera_2d.make_current()

@rpc("call_local", "any_peer")
func take_control() -> void:
	if controlling_peer==0 and istate.faction is PlayerFaction and istate.race is ControllableRace:
		controlling_peer = multiplayer.get_remote_sender_id()
		set_multiplayer_authority(multiplayer.get_remote_sender_id())
		if multiplayer.get_unique_id() == multiplayer.get_remote_sender_id():
			camera_2d.make_current()

@rpc("call_local", "any_peer") 
func free_control() -> void: 
	if controlling_peer == multiplayer.get_remote_sender_id(): controlling_peer = 0
		
func _input(event: InputEvent) -> void:
	if controlling_peer==multiplayer.get_unique_id() and event.is_pressed():
		if event is InputEventMouseButton:
			if Config.enable_change_zoom:
				if event.is_action(&"wheel_down"):
					camera_2d.zoom *= 0.9
				elif event.is_action(&"wheel_up"):
					camera_2d.zoom *= 1.1
				if Config.enable_zoom_limit and not Config.debug:
					camera_2d.zoom = camera_2d.zoom.clamp(Config.zoom_out_max, Config.zoom_in_max)
			
		if Config.debug and event.is_action(&"f1"):
			print((get_parent() as TileMap).local_to_map(position))

func _process(delta: float) -> void:
	var my_peer:int = multiplayer.get_unique_id()
	match [is_multiplayer_authority(), controlling_peer]:
		[_, my_peer]: _update_direction_axis_by_input(delta)
		[true, _]: ai_control(delta)
	_update_distance_moved()
	_update_body_state()
	_play_animation()

var distance_moved: float; var _previous_position: Vector2 = position
func _update_distance_moved() -> void:
	distance_moved = position.distance_to(_previous_position)
	_previous_position = position
	
func _update_body_state() -> void: 	
	if distance_moved > 1:
		_adjust_speed_scale(distance_moved, 1)
		_change_body_state(Enums.AnimationState.JOG)
	elif distance_moved > 0.01:
		_adjust_speed_scale(distance_moved, 0.8)
		_change_body_state(Enums.AnimationState.WALK)
	else:
		_change_body_state(Enums.AnimationState.IDLE)

var _body_state: Enums.AnimationState = Enums.AnimationState.IDLE
		
var _faced_dir: Enums.Dir = Enums.Dir.DOWN

func _change_body_state(new_body_state: Enums.AnimationState):
	_body_state = new_body_state
func _adjust_speed_scale(distance_moved: float, factor: float):
	for body_part in body_holder.get_children():
		if body_part is AnimatedBodyPortion:
			body_part.speed_scale = distance_moved/factor
		
func ai_control(delta: float): 
	ai_process.behave(delta)
	_update_velocity(delta)
	
var _direction_axis: Vector2 = Vector2.ZERO

var distance_moved_since_load: float = 501
func _update_direction_axis_by_input(delta: float) -> void:
	
	_direction_axis = Input.get_vector(&"ui_left", &"ui_right", &"ui_up", &"ui_down")
	
	_update_velocity(delta)
	
	distance_moved_since_load += distance_moved
	
	if distance_moved_since_load > 500:
		load_tiles_around_me.emit(position, Vector2i(192, 120), uid)#195, 120
		distance_moved_since_load = 0
		
func apply_friction(amount: float, delta: float):
	velocity = velocity.move_toward(Vector2.ZERO, amount * delta)

func _update_velocity(delta: float):
	apply_friction(friction, delta)
	if _direction_axis != Vector2.ZERO:
		_direction_axis = _direction_axis.normalized()
		velocity += _direction_axis * acceleration * delta
		
		velocity = velocity.limit_length(istate.get_max_speed())
		_update_faced_dir(_direction_axis)
		if controlling_peer==0 or not (Config.noclip and controlling_peer > 0):
			move_and_slide()
		else:
			position += _direction_axis * Config.noclip_speed
		
func _update_faced_dir(direction: Vector2) -> void:
	var new_dir: Enums.Dir
	if abs(direction.x) > abs(direction.y): 
		new_dir = Enums.Dir.LEFT if direction.x < 0 else Enums.Dir.RIGHT
	else: 
		new_dir = Enums.Dir.UP if direction.y < 0 else Enums.Dir.DOWN
	
	if new_dir != _faced_dir: _setsync_faced_dir.rpc(new_dir)

@rpc("call_local", "any_peer") func _setsync_faced_dir(new_dir: Enums.Dir): 
	if is_multiplayer_authority() or controlling_peer == multiplayer.get_remote_sender_id():
		_faced_dir = new_dir
		
func _play_animation() -> void:	
	for body_part in body_holder.get_children():
		if body_part is AnimatedBodyPortion and body_part.sprite_frames:
			body_part._play_handled(_body_state, _faced_dir)
			
func serialize() -> Dictionary:#guardar como packedscene en vez de esto
	return {
		&"direction": _faced_dir,
		&"position": position,
		&"state": istate.serialize()
	}

@rpc("call_local")func setsync_pos_reliable(loc_pos: Vector2): position=loc_pos; _previous_position=loc_pos
@rpc("call_local")func setsync_node_name_and_uid(_uid:int):self.name="%d%s%s"%[_uid,istate.race.name,name_label.text];self.uid=_uid;

func distance_to(thing: Node2D) -> float: return self.global_position.distance_to(thing.global_position)
	
