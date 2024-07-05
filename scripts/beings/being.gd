extends CharacterBody2D

class_name Being
#para persistirlo habrá que usar packedscene para guardar las cosas custom children q puede ser q tenga, sino habrá q iterar por cada child

var uid: int = randi_range(-9223372036854775808, 9223372036854775807)

@export var acceleration = 2500

@export var friction = 1600 #hacer q provenga de la tile en custom data

@onready var body_holder: Node2D = $BodyHolder
@onready var camera_2d: Camera2D = $Camera2D

@onready var internal_state: BeingInternalState = $InternalState

signal load_tiles_around_me(coords: Vector2, chunk_size: Vector2i, uid: int)

@onready var body: AnimatedBodyPortion = $BodyHolder/Body
@onready var head: AnimatedBodyPortion = $BodyHolder/Head
@onready var nav = $NavigationAgent2D
@onready var ai_behavior = $AiBehavior
@onready var name_label = $NameLabel

#NO BORRAR
func _ready():
	_connect_tile_map()

func _connect_tile_map():
	var tile_map: RustTileMap = get_parent()
	load_tiles_around_me.connect(tile_map.load_tiles_around)

#constructs for multiplayer too
func construct(preiniter: BeingStatePreIniter) -> void:
	if preiniter.sprite_body:
		body.construct(preiniter.sprite_body, preiniter.body_scale)
		if preiniter.sprite_head:
			head.construct(preiniter.sprite_head, preiniter.head_scale, preiniter.sprite_body.head_v_offset, preiniter.body_scale.z)
			
	internal_state.construct_from_seri.rpc(preiniter.internal_state.serialize())
	if internal_state.beingkind:
		if internal_state.beingkind.ai_process:
			ai_behavior.set_script(internal_state.beingkind.ai_process)	
	else:
		ai_behavior.set_script(internal_state.race.ai_process)
	
	if internal_state.faction is PlayerFaction or (internal_state.beingkind and internal_state.beingkind.display_being_name):
		set_name_label_text_and_color.rpc(preiniter.name, internal_state.faction.name_label_color)
		#TODO key press para ocultar las namelabels de todos (usar el grupo)
	
@rpc("call_local") func set_name_label_text_and_color(text: String, color: Color): name_label.text = text; name_label.label_settings.font_color = color

var uncontrolled: bool = true

@rpc("call_local")
func give_control(peer_id: int) -> void:
	if internal_state.faction is PlayerFaction and internal_state.race is ControllableRace:
		uncontrolled = false
		set_multiplayer_authority(peer_id)
		if peer_id == multiplayer.get_unique_id() and internal_state.faction is PlayerFaction:
			camera_2d.make_current()

@rpc("call_local", "any_peer")
func take_control() -> void:
	if internal_state.faction is PlayerFaction and uncontrolled and internal_state.race is ControllableRace:
		uncontrolled = false
		set_multiplayer_authority(multiplayer.get_remote_sender_id())
		if multiplayer.get_unique_id() == multiplayer.get_remote_sender_id():
			camera_2d.make_current()

@rpc("call_local") 
func free_control() -> void: uncontrolled = true
		
var zoom_min = 1.51*Vector2.ONE; var zoom_max = 999*Vector2.ONE		
		
func _input(event: InputEvent) -> void:
	if is_multiplayer_authority() and event.is_pressed():
		
		if event is InputEventMouseButton:
			if Config.enable_change_zoom:
				if event.is_action("wheel_down"):
					camera_2d.zoom *= 0.9
				elif event.is_action("wheel_up"):
					camera_2d.zoom *= 1.1
				camera_2d.zoom = camera_2d.zoom.clamp(zoom_min, zoom_max)
			
		if Config.debug and event.is_action("f1"):
			print((get_parent() as TileMap).local_to_map(position))

func _physics_process(delta: float) -> void:
	match [is_multiplayer_authority(), internal_state.faction is PlayerFaction, uncontrolled]:
		[false, ..]:
			return
		[_, false, _]:
			wall_min_slide_angle = 0
			ai_control()
		[_, true, false]:
			_update_direction_axis_by_input(delta)
		[_, true, true]:
			wall_min_slide_angle = 0
			owned_ai_control()
			
	_update_distance_moved()
	_update_body_state()
	
	_process_animation()

var distance_moved: float; var _previous_position: Vector2 = position
func _update_distance_moved() -> void:
	distance_moved = position.distance_to(_previous_position)
	_previous_position = position
	
func _update_body_state() -> void: 	
	if distance_moved > 1:
		_adjust_speed_scale.rpc(distance_moved, 1)
		_change_body_state.rpc(Enums.AnimationState.JOG)
	elif distance_moved > 0.01:
		_adjust_speed_scale.rpc(distance_moved, 0.8)
		_change_body_state.rpc(Enums.AnimationState.WALK)
	else:
		_change_body_state.rpc(Enums.AnimationState.IDLE)
	

var _body_state: Enums.AnimationState = Enums.AnimationState.IDLE
		
var _facing_direction: Enums.Dir = Enums.Dir.DOWN

@rpc("call_local", "unreliable")
func _change_body_state(new_body_state: Enums.AnimationState):
	_body_state = new_body_state
@rpc ("call_local", "unreliable")
func _adjust_speed_scale(distance_moved: float, factor: float):
	for body_part in body_holder.get_children():
		if body_part is AnimatedBodyPortion:
			body_part.speed_scale = distance_moved/factor
		
func owned_ai_control(): pass		
func ai_control(): pass #q llame a un @export script que este en beingkind
	
var _direction_axis: Vector2 = Vector2.ZERO

var distance_moved_since_load: float = 501
func _update_direction_axis_by_input(delta: float) -> void:
	
	_direction_axis = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	
	apply_friction(friction, delta)
	
	if _direction_axis != Vector2.ZERO:
		_direction_axis = _direction_axis.normalized()
		velocity += _direction_axis * acceleration * delta
		
		velocity = velocity.limit_length(internal_state.get_max_speed())
		_update_facing_direction()
	
	if not Config.noclip:
		move_and_slide()
	else:
		position += _direction_axis * Config.noclip_speed
	
	distance_moved_since_load += distance_moved
	
	if distance_moved_since_load > 500:
		load_tiles_around_me.emit(position, Vector2i(192, 120), uid)#195, 120
		distance_moved_since_load = 0
		
func apply_friction(amount: float, delta: float):
	velocity = velocity.move_toward(Vector2.ZERO, amount * delta)
		
func _update_facing_direction() -> void:
	if abs(_direction_axis.x) > abs(_direction_axis.y): 
		_facing_direction = Enums.Dir.LEFT if _direction_axis.x < 0 else Enums.Dir.RIGHT
	else: 
		_facing_direction = Enums.Dir.UP if _direction_axis.y < 0 else Enums.Dir.DOWN

# esto debería ser un componente?
func _process_animation() -> void:	
	_play_animation(_body_state, _facing_direction)

func _play_animation(animation_state: Enums.AnimationState, direction: Enums.Dir) -> void:	
	for body_part in body_holder.get_children():
		if body_part is AnimatedBodyPortion and body_part.sprite_frames:
			body_part._play_handled(animation_state, direction)
			
func serialize() -> Dictionary:#guardar como packedscene en vez de esto
	return {
		"direction": _facing_direction,
		"position": position,
		"state": internal_state.serialize()
	}

@rpc("call_local")
func sync_pos_reliable(loc_coords: Vector2):
	position = loc_coords
