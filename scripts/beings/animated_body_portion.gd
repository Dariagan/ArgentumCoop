extends AnimatedSprite2D
class_name AnimatedBodyPortion

var _animation_states: Array[StringName]
var _starting_sprite_width_frontally_sideways_height
var _character_width_frontally_sideways_height: Vector3

#_scale vector: frontal width, side width, and height
var _scale: Vector3 = Vector3.ONE: 
	set(value):
		if value != Vector3.ONE:
			_scale = value
			_scale_altered = true
		else:
			_scale = value
			_scale_altered = false

var _scale_altered: bool = false


func construct(sprite_data: SpriteData = null, character_width_frontally_sideways_height: Vector3 = Vector3.ONE) -> void:
	_set_starting_sprite_data(sprite_data)
	_starting_sprite_width_frontally_sideways_height = sprite_data.width_frontally_sideways_height
	_character_width_frontally_sideways_height = character_width_frontally_sideways_height
	
	var net_scale: Vector3 = _starting_sprite_width_frontally_sideways_height * _character_width_frontally_sideways_height
	self.set("_scale", net_scale)

# externally, change the frames instead of the sprite data
func _set_starting_sprite_data(sprite_data: SpriteData) -> void:
	assert (sprite_data != null)
	self.name = sprite_data.name
	sprite_frames = sprite_data.frames
	_animation_states = sprite_data.animation_states
	position = sprite_data.offset_global

func change_sprite_data(sprite_data: SpriteData) -> void:
	assert (sprite_data != null)
	sprite_frames = sprite_data.frames
	_animation_states = sprite_data.animation_states
	
	var net_scale: Vector3 = _starting_sprite_width_frontally_sideways_height * _character_width_frontally_sideways_height * sprite_data.width_frontally_sideways_height
	self.set("_scale", net_scale)

# DON'T CALL "PLAY" WITHOUT A _ BY ACCIDENT
func _play(animation_name: String = "", custom_speed: float = 1.0, from_end: bool = false) -> void:
	var split_string: PackedStringArray = animation_name.split("_", false, 1)
	var animation_state: StringName = split_string[0]
	var animation_direction: StringName = split_string[1]
	
	if _scale_altered:
		match animation_direction:
			&"up", &"down":
				super.apply_scale(Vector2(_scale[0], _scale[2]))
			&"left", &"right":
				super.apply_scale(Vector2(_scale[1], _scale[2]))
	
	if animation_state in _animation_states:
		_play_remotely.rpc(animation_name, custom_speed, from_end)
	else:	
		_play_remotely.rpc(animation_name.replace(animation_state, "idle"), custom_speed, from_end)

#sacar lo de any peer y asignar authority
@rpc("call_local", "any_peer")
func _play_remotely(animation_name: String, custom_speed: float, from_end: bool):
	super.play(animation_name, custom_speed, from_end)

#sacar lo de any peer y asignar authority
@rpc("call_local", "any_peer")
func _apply_scale_remotely(given_scale: Vector2):
	super.apply_scale(Vector2(given_scale))
