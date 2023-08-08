extends CharacterBody2D
class_name Being
#así añadimos nueva funcionalidad en base a composición en vez de a herencia (sostenible a largo plazo) 

@export var speed: float = 6
@export var acceleration = 30

@export var friction = 16 #hacer q provenga del piso

@onready var body_holder: Node2D = $BodyHolder
@onready var camera_2d: Camera2D = $Camera2D

enum BodyState { IDLE, WALK, JOG }

var body_state: BodyState = BodyState.IDLE
		
var _facing_direction: String = "down"


@export var animated_portion_scene: PackedScene

var animated_parts_to_add_to_scene: Array[AnimatedBodyPortion] = []

func construct(data: BeingSpawnData) -> void:
	_add_body_and_head(data)
	
func _add_body_and_head(data: BeingSpawnData):#v_offset: float = 1 (se lo da el body)
	if data.body_i > -1:
		var body_portion: AnimatedBodyPortion = animated_portion_scene.instantiate()
		var body_sprite_data: BodySpriteData = data.race.body_sprites_datas[data.body_i]
		
		body_holder.add_child(body_portion)
		body_portion.construct(body_sprite_data, data.body_scale)
		_set_frames_remotely(body_portion, data.race.id, data.body_i)
		
		if data.head_i > -1:
			var head_portion = animated_portion_scene.instantiate()
			body_holder.add_child(head_portion)
			head_portion.construct(data.race.head_sprites_datas[data.head_i], data.head_scale, body_sprite_data.head_v_offset ,data.body_scale.z)
			_set_frames_remotely(head_portion, data.race.id, data.head_i, true)
@rpc
func _set_frames_remotely(portion: AnimatedBodyPortion, race_id: String, i: int, head: bool = false):
	match [race_id.begins_with("controllable_"), head]:
		[true, false]: portion.sprite_frames = (GlobalGameData.controllable_races[race_id] as ControllableRace).body_sprites_datas[i].frames
		[true, true]: portion.sprite_frames = (GlobalGameData.controllable_races[race_id] as ControllableRace).head_sprites_datas[i].frames
		[false, false]: portion.sprite_frames = (GlobalGameData.uncontrollable_races[race_id] as UncontrollableRace).body_sprites_datas[i].frames
		[false, true]: portion.sprite_frames = (GlobalGameData.uncontrollable_races[race_id] as UncontrollableRace).head_sprites_datas[i].frames
	
		
func asd_enter_tree() -> void:
	set_multiplayer_authority(name.to_int())
	print("controlado por %s" % name.to_int())
	
func cede_authority(peer_id: int) -> void:
	set_multiplayer_authority(peer_id)

func _physics_process(delta: float) -> void:
	if is_multiplayer_authority():
		move_by_input(delta)
		camera_2d.make_current()
	else:
		pass #AI control
	
var _input_axis: Vector2 = Vector2.ZERO
var _velocity: Vector2 = Vector2.ZERO
var previous_position: Vector2 = position
func move_by_input(delta: float) -> void:
	
	_input_axis = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	
	body_state = BodyState.IDLE
	
	apply_friction(friction, delta)
	
	if _input_axis != Vector2.ZERO:
		_input_axis = _input_axis.normalized()
		_velocity += _input_axis * acceleration * delta
		_velocity = _velocity.limit_length(speed)
		_update_facing_direction()
	
	move_and_collide(_velocity)
	
	var distance_moved: float = position.distance_to(previous_position)

	if distance_moved > 1:
		for body_part in body_holder.get_children():
			body_part.speed_scale = distance_moved/1
		body_state = BodyState.JOG
		
	elif distance_moved > 0.01:
		for body_part in body_holder.get_children():
			body_part.speed_scale = distance_moved/0.8
		body_state = BodyState.WALK
		
	else:
		body_state = BodyState.IDLE
	
	_process_animation()
	
	previous_position = position
	
func apply_friction(amount: float, delta: float):
	var real_amount: float = amount * delta
	
	# _velocity = _velocity.move_toward(Vector2.ZERO, real_amount)
	if _velocity.length() > real_amount:
		_velocity -= _velocity.normalized() * real_amount
	else:
		_velocity = Vector2.ZERO
		
func _update_facing_direction() -> void:
	if abs(_input_axis.x) > abs(_input_axis.y): 
		_facing_direction = "left" if _input_axis.x < 0 else "right"
	else: 
		_facing_direction = "up" if _input_axis.y < 0 else "down"

# esto debería ser un componente
func _process_animation() -> void:	
	_play_animation(str(BodyState.keys()[body_state]).to_lower()
	 + "_" + _facing_direction)

func _play_animation(animation_name: String) -> void:	
	
	#creo q es mejor
	for body_part in body_holder.get_children():
		if body_part.sprite_frames:
			body_part._play_handled(animation_name)
