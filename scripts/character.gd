extends CharacterBody2D
class_name Character#Being, después crear otros scripts con su class_name q lo extiendan (animal y humanoid?).
#después, extender a animal y humanoid y meter los componentes que quieras que definiste en otros scripts.
#así añadimos nueva funcionalidad en base a composición en vez de a herencia (sostenible a largo plazo) 

@export var speed: float = 6
@export var acceleration = 30


@export var friction = 16 #hacer q salga del piso


@onready var body_parts: Node2D = $BodyParts


@onready var camera_2d: Camera2D = $Camera2D

var _velocity: Vector2 = Vector2.ZERO


var axis: Vector2 = Vector2.ZERO



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
		for body_part in body_parts.get_children():
			body_part.speed_scale = distance_moved/1
		body_state = BodyState.JOG
		
	elif distance_moved > 0.01:
		for body_part in body_parts.get_children():
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
	if abs(axis.x) > abs(axis.y):
		_facing_direction = "left" if axis.x < 0 else "right"
	else:
		_facing_direction = "up" if axis.y < 0 else "down"

# esto debería ser un componente
func _process_animation() -> void:	
	_play_animation.rpc(str(BodyState.keys()[body_state]).to_lower()
	 + "_" + _facing_direction)
@rpc("call_local")
func _play_animation(animation_name: String) -> void:	
	
	#creo q es mejor
	for body_part in body_parts.get_children():
		if body_part.sprite_frames:
			body_part.play(animation_name)
			#agregar un Node nombrado idle_only o un script a la body part con un bool q indique si es idle_only=true, en ese caso usar el replace
			#puede q haya q hacer algo parecido si es solo run sin jog, asi q tal vez el script con un array es mejor. si el 
			#TAL VEZ es mejor llamar a algun método con parámetro un enum-estado en cada body part scripteada y esta handlee la animación q debería playear
	"""
	if body.sprite_frames:
		body.play(animation_name)
	if weapon.sprite_frames:
		weapon.play(animation_name)
	if shield.sprite_frames:
		shield.play(animation_name)
	if head.sprite_frames:
		head.play(animation_name.replace("walk", "idle").replace("jog", "idle"))
	if helmet.sprite_frames:
		helmet.play(animation_name.replace("walk", "idle").replace("jog", "idle"))
	"""
# esto debería ser un componente
