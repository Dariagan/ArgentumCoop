extends CharacterBody2D
class_name Character

@export var speed: float = 6
@export var acceleration = 30


@export var friction = 16 #hacer q salga del piso

@export var _body_sprite: AnimatedSprite2D 
@export var _head_sprite: AnimatedSprite2D 
@export var _helmet_sprite: AnimatedSprite2D 
@export var _weapon_sprite: AnimatedSprite2D
@export var _shield_sprite: AnimatedSprite2D

@onready var camera_2d: Camera2D = $Camera2D

var _velocity: Vector2 = Vector2.ZERO


var axis: Vector2 = Vector2.ZERO

var character: CharacterData

enum BodyState { IDLE, WALK, JOG }

var body_state: BodyState = BodyState.IDLE
		
var _facing_direction: String = "down"

var previous_position: Vector2 = position

func _enter_tree() -> void:
	set_multiplayer_authority(name.to_int())
	print("controlado por %s" % name.to_int())
	
func take_control(peer_id: int) -> void:
	set_multiplayer_authority(peer_id)

func _physics_process(delta: float) -> void:
	if is_multiplayer_authority():
		move_by_input(delta)
		camera_2d.make_current()
	else:
		pass #AI control
	
func move_by_input(delta: float) -> void:
	
	axis = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	
	body_state = BodyState.IDLE
	
	apply_friction(friction, delta)
	
	if axis != Vector2.ZERO:
		axis = axis.normalized()
		_velocity += axis * acceleration * delta
		_velocity = _velocity.limit_length(speed)
		_update_facing_direction()
	
	move_and_collide(_velocity)
	
	var distance_moved: float = position.distance_to(previous_position)

	if distance_moved > 1:
		_body_sprite.speed_scale = distance_moved/1
		body_state = BodyState.JOG
	elif distance_moved > 0.01:
		_body_sprite.speed_scale = distance_moved/0.8
		body_state = BodyState.WALK
	else:
		body_state = BodyState.IDLE
	
	_process_animation()
	
	previous_position = position

func apply_friction(amount: float, delta: float):
	var real_amount: float = amount * delta
	if _velocity.length() > real_amount:
		_velocity -= _velocity.normalized() * real_amount
	else:
		_velocity = Vector2.ZERO
		
func _update_facing_direction() -> void:
	if abs(axis.x) > abs(axis.y):
		_facing_direction = "left" if axis.x < 0 else "right"
	else:
		_facing_direction = "up" if axis.y < 0 else "down"

func _process_animation() -> void:	
	_play_animation.rpc(str(BodyState.keys()[body_state]).to_lower()
	 + "_" + _facing_direction)
	
@rpc("call_local")
func _play_animation(animation_name: String) -> void:	
	if _body_sprite.sprite_frames:
		_body_sprite.play(animation_name)
	if _weapon_sprite.sprite_frames:
		_weapon_sprite.play(animation_name)
	if _shield_sprite.sprite_frames:
		_shield_sprite.play(animation_name)
	if _head_sprite.sprite_frames:
		_head_sprite.play(animation_name.replace("walk", "idle").replace("jog", "idle"))
	if _helmet_sprite.sprite_frames:
		_helmet_sprite.play(animation_name.replace("walk", "idle").replace("jog", "idle"))
