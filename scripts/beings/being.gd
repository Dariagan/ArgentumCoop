extends CharacterBody2D
class_name Being
#para persistirlo habrá que usar packedscene para guardar las cosas custom children q puede ser q tenga, sino habrá q iterar por cada child
var uid: int
var maccel = 2500
var mai_process: AiProcess = AiProcess.new(self)

var friction = 1600 #hacer q provenga de la tile en custom data
@onready var mcamera: Camera2D = $Camera2D; @onready var body_holder: Node2D = $BodyHolder
@onready var mistate: BeingInternalState = $InternalState
@onready var mbody: AnimatedBodyPortion = $BodyHolder/Body; @onready var head: AnimatedBodyPortion = $BodyHolder/Head
@onready var mnav: NavigationAgent2D = $NavigationAgent2D; @onready var name_label = $NameLabel; 
@onready var mtilemap: GdTileMap = get_parent()
const CHUNK_SIZE: Vector2 = Vector2i(192, 120)
var mcontroller_speed_multiplier: float = 1

#constructs for multiplayer too
func construct(preiniter: BeingStatePreIniter, uid_: int) -> void:
	if preiniter.msprite_body:
		mbody.construct(preiniter.msprite_body, preiniter.mbody_scale)
		if preiniter.msprite_head:
			head.construct(preiniter.msprite_head, preiniter.mhead_scale, preiniter.msprite_body.head_v_offset, preiniter.mbody_scale.z)
	mistate.construct_from_seri.rpc(preiniter.mistate.serialize())
	set_ai_process.rpc()
	var show_label: bool = mistate.mfaction is PlayerFaction or (mistate.mbeing_gen_template and mistate.being_gen_template.display_being_name)
	set_name_label_text_and_color.rpc(preiniter.mname, mistate.mfaction.mcolor, show_label)
	self.setsync_node_name_and_uid.rpc(uid_)
	#TODO key press para ocultar las namelabels de todos (usar el grupo)

@rpc("call_local")
func set_ai_process():
	if mistate.mbeing_gen_template and mistate.mbeing_gen_template.mai_process:
		mai_process = mistate.mbeing_gen_template.mai_process.new(self)
	elif mistate.mrace.mai_process:
		mai_process = mistate.mrace.mai_process.new(self)

@rpc("call_local") func set_name_label_text_and_color(text: String, color: Color, show_label: bool): 
	var ui_show_labels: bool = true
	
	name_label.text = text; name_label.visible = show_label and ui_show_labels
	name_label.label_settings = name_label.label_settings.duplicate(); name_label.label_settings.font_color = color; 

var mcontrolling_peer: int = 0 : set = set_controlling_peer
func set_controlling_peer(peer: int): mcontrolling_peer = max(0, peer); if mcontrolling_peer == 0: wall_min_slide_angle = 0

@rpc("call_local", "any_peer")
func give_control(new_controller_peer: int) -> void:
	if (mcontrolling_peer==0 or multiplayer.get_remote_sender_id() == mcontrolling_peer)\
	   and mistate.mfaction is PlayerFaction and mistate.mrace is ControllableRace:
		set_multiplayer_authority(new_controller_peer)
		mcontrolling_peer = new_controller_peer
		if new_controller_peer == multiplayer.get_unique_id():
			mcamera.make_current()

@rpc("call_local", "any_peer")
func take_control() -> void:
	if mcontrolling_peer==0 and mistate.mfaction is PlayerFaction and mistate.mrace is ControllableRace:
		mcontrolling_peer = multiplayer.get_remote_sender_id()
		set_multiplayer_authority(multiplayer.get_remote_sender_id())
		if multiplayer.get_unique_id() == multiplayer.get_remote_sender_id():
			mcamera.make_current()

@rpc("call_local", "any_peer") 
func free_control() -> void: 
	if mcontrolling_peer == multiplayer.get_remote_sender_id(): mcontrolling_peer = 0
		
func _input(event: InputEvent) -> void:
	if mcontrolling_peer==multiplayer.get_unique_id() and event.is_pressed():
		if event is InputEventMouseButton:
			if Config.enable_change_zoom:
				if event.is_action(&"wheel_down"):
					mcamera.zoom *= 0.9
				elif event.is_action(&"wheel_up"):
					mcamera.zoom *= 1.1
				if Config.enable_zoom_limit and not Config.debug:
					mcamera.zoom = mcamera.zoom.clamp(Config.zoom_out_max, Config.zoom_in_max)
			
		if Config.debug and event.is_action(&"f1"):
			print(mtilemap.local_to_tilemap(position))

func _process(delta: float) -> void:
	var my_peer:int = multiplayer.get_unique_id()
	match [is_multiplayer_authority(), mcontrolling_peer]:
		[_, my_peer]: player_control(delta)
		[true, _]: ai_control(delta)
	_update_distance_moved()
	_update_body_state()
	_play_animation()

var mdistance_moved: float; var mprevious_pos: Vector2 = position
func _update_distance_moved() -> void:
	mdistance_moved = position.distance_to(mprevious_pos)
	mprevious_pos = position
	
func _update_body_state() -> void: 	
	if mdistance_moved > 1: _adjust_speed_scale(1); _change_body_state(Enums.AnimationState.JOG)
	elif mdistance_moved > 0.01: _adjust_speed_scale(0.8); _change_body_state(Enums.AnimationState.WALK)
	else: _change_body_state(Enums.AnimationState.IDLE)

var mbody_state: Enums.AnimationState = Enums.AnimationState.IDLE
var mfaced_dir: Enums.Dir = Enums.Dir.DOWN

func _change_body_state(new_body_state: Enums.AnimationState):
	mbody_state = new_body_state
func _adjust_speed_scale(factor: float):
	for body_part in body_holder.get_children():
		if body_part is AnimatedBodyPortion:
			body_part.speed_scale = mdistance_moved/factor
		
func ai_control(delta: float): mai_process.behave(delta); _update_velocity(delta)
	
var _direction_axis: Vector2 = Vector2.ZERO

var distance_moved_since_load: float = 501
func player_control(delta: float) -> void:
	_direction_axis = Input.get_vector(&"ui_left", &"ui_right", &"ui_up", &"ui_down")
	mcontroller_speed_multiplier = 1.0
	_update_velocity(delta)
	distance_moved_since_load += mdistance_moved
	if distance_moved_since_load > 500:
		mtilemap.load_tiles_around(mtilemap.local_to_tilemap(position), CHUNK_SIZE, uid)#195, 120
		distance_moved_since_load = 0
		
func apply_friction(amount: float, delta: float):
	velocity = velocity.move_toward(Vector2.ZERO, amount * delta)

func _update_velocity(delta: float):
	apply_friction(friction, delta)
	if _direction_axis != Vector2.ZERO:
		_direction_axis = _direction_axis.normalized()
		velocity += _direction_axis*maccel*delta
		velocity = velocity.limit_length(mistate.get_max_speed()*mcontroller_speed_multiplier)
		_update_faced_dir(_direction_axis)
		if mcontrolling_peer==0 or not (Config.noclip and mcontrolling_peer > 0):
			move_and_slide()
		else:
			position += _direction_axis * Config.noclip_speed
		
func _update_faced_dir(direction: Vector2) -> void:
	var new_dir: Enums.Dir
	if abs(direction.x) > abs(direction.y): new_dir = Enums.Dir.LEFT if direction.x < 0 else Enums.Dir.RIGHT
	else: new_dir = Enums.Dir.UP if direction.y < 0 else Enums.Dir.DOWN
	if new_dir != mfaced_dir: _setsync_faced_dir.rpc(new_dir)

@rpc("call_local") func _setsync_faced_dir(new_dir: Enums.Dir): mfaced_dir = new_dir
		
func _play_animation() -> void:	
	for body_part in body_holder.get_children():
		if body_part is AnimatedBodyPortion and body_part.sprite_frames:
			body_part._play_handled(mbody_state, mfaced_dir)
			
func serialize() -> Dictionary:#guardar como packedscene en vez de esto
	return {&"direction": mfaced_dir, &"position": position, &"state": mistate.serialize()}

@rpc("call_local")func setsync_pos_reliable(loc_pos: Vector2): position=loc_pos; mprevious_pos=loc_pos
@rpc("call_local")func setsync_node_name_and_uid(_uid:int):self.name="%d%s%s"%[_uid,mistate.mrace.mname,name_label.text];self.uid=_uid;

func distance_to(thing: Node2D) -> float: return self.global_position.distance_to(thing.global_position)
