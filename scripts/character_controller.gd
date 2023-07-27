extends CharacterBody2D

@export var speed = 160
@export var acceleration = 3000
@export var friction = 10000

@export var _body_sprite: AnimatedSprite2D 
@export var _head_sprite: AnimatedSprite2D 
@export var _helmet_sprite: AnimatedSprite2D 
@export var _weapon_sprite: AnimatedSprite2D
@export var _shield_sprite: AnimatedSprite2D

@onready var camera_2d: Camera2D = $Camera2D


var axis: Vector2 = Vector2.ZERO

var character: Character 

func _enter_tree() -> void:
	set_multiplayer_authority(name.to_int())
	
func take_control() -> void:
	set_multiplayer_authority(name.to_int())

func _physics_process(delta: float) -> void:
	if is_multiplayer_authority():
		move_by_input(delta)
		camera_2d.make_current()
		
	
func move_by_input(delta: float) -> void:
	
	axis = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	
	if axis == Vector2.ZERO:
		apply_friction(friction * delta)
		is_moving = false
	else:
		is_moving = true
		_update_facing_direction()
		apply_movement(axis.normalized() * acceleration * delta)
	
	_process_animation()
	move_and_slide()
	
func apply_movement(accel: Vector2):
	velocity += accel
	velocity = velocity.limit_length(speed)

func apply_friction(amount):
	if velocity.length() > amount:
		velocity -= velocity.normalized() * amount
	else:
		velocity = Vector2.ZERO
		
var is_moving: bool = false
var _facing_direction: String = "down"
		
func _update_facing_direction() -> void:
	if abs(axis.x) > abs(axis.y):
		_facing_direction = "left" if axis.x < 0 else "right"
	else:
		_facing_direction = "up" if axis.y < 0 else "down"

func _process_animation() -> void:
	var state = "walk" if is_moving else "idle"	
	_play_animation.rpc(state + "_" + _facing_direction)
	
@rpc("call_local")
func _play_animation(animation_name: String) -> void:	
	if _body_sprite.sprite_frames:
		_body_sprite.play(animation_name)
	if _weapon_sprite.sprite_frames:
		_weapon_sprite.play(animation_name)
	if _shield_sprite.sprite_frames:
		_shield_sprite.play(animation_name)
	if _head_sprite.sprite_frames:
		_head_sprite.play(animation_name.replace("walk", "idle"))
	if _helmet_sprite.sprite_frames:
		_helmet_sprite.play(animation_name.replace("walk", "idle"))
