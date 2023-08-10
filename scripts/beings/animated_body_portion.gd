extends AnimatedSprite2D
class_name AnimatedBodyPortion

var _animation_states: Array
var _starting_sprite_width_frontally_sideways_height: Vector3
var _character_width_frontally_sideways_height: Vector3
var _vertical_offset: float = 0
var _vertical_offset_mult: float = 1

#_scale vector: frontal width, side width, and height
var _scale: Vector3 = Vector3.ONE
			
@rpc("call_local")
func _set_scale_remotely(value: Vector3):
	_scale = value
	scale.y = value.z
	_width_asimetrically_altered = value.x != value.y
	if not _width_asimetrically_altered:
		scale.x = value.x
		
var _width_asimetrically_altered: bool = false

func construct(sprite_data: SpriteData, character_width_frontally_sideways_height: Vector3, vertical_offset: float = 0, vertical_offset_mult:float = 1) -> void:
	_vertical_offset = vertical_offset
	_vertical_offset_mult = vertical_offset_mult
	_starting_sprite_width_frontally_sideways_height = sprite_data.width_frontally_sideways_height
	_character_width_frontally_sideways_height = character_width_frontally_sideways_height
	_set_starting_sprite_data(sprite_data)

	_set_scale_remotely.rpc(_starting_sprite_width_frontally_sideways_height * _character_width_frontally_sideways_height)

# externally, change the frames instead of the sprite data
func _set_starting_sprite_data(sprite_data: SpriteData) -> void:
	assert (sprite_data != null)
	sprite_frames = sprite_data.frames
	for state in sprite_data.animation_states:
		_animation_states.push_back(state)
	position = sprite_data.offset_global
	position.y += _vertical_offset
	position.y *= pow(_vertical_offset_mult, 1/1.5)
	_set_starting_sprite_data_remotely.rpc([sprite_data.id, _animation_states, sprite_data.offset_global, position.y])

@rpc
func _set_starting_sprite_data_remotely(data: Array):
	sprite_frames = GlobalData.sprites_datas[data[0]].frames
	_animation_states = data[1]
	position = data[2]
	position.y = data[3]

func change_sprite_data(sprite_data: SpriteData) -> void:
	assert (sprite_data != null)
	sprite_frames = sprite_data.frames
	_animation_states = sprite_data.animation_states
	
	var net_scale: Vector3 = _starting_sprite_width_frontally_sideways_height * _character_width_frontally_sideways_height * sprite_data.width_frontally_sideways_height
	self.set("_scale", net_scale)


func _play_handled(animation_name: String = "", custom_speed: float = 1.0, from_end: bool = false) -> void:
	var split_string: PackedStringArray = animation_name.split("_", false, 1)
	var animation_state: StringName = split_string[0]
	var animation_direction: StringName = split_string[1]
	
	if _width_asimetrically_altered:
		match animation_direction:
			&"up", &"down":
				_set_x_scale_remotely.rpc(_scale[0])
			&"left", &"right":
				_set_x_scale_remotely.rpc(_scale[1])
	
	if animation_state in _animation_states:
		_play_remotely.rpc(animation_name, custom_speed, from_end)
	else:	
		_play_remotely.rpc(animation_name.replace(animation_state, "idle"), custom_speed, from_end)

#sacar lo de any peer y asignar authority
@rpc("call_local", "any_peer", "unreliable")
func _play_remotely(animation_name: String, custom_speed: float, from_end: bool):
	super.play(animation_name, custom_speed, from_end)

#sacar lo de any peer y asignar authority
@rpc("call_local", "any_peer", "unreliable")
func _set_x_scale_remotely(value: float):
	scale.x = value
