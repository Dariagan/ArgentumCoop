extends AnimatedSprite2D
class_name AnimatedBodyPortion
#no usar synchronizer para esto porq sino hay q hacer configuraci

var _avai_animation_states: PackedInt32Array
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

#construye multiplayer tambien
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
		_avai_animation_states.push_back(state)
	_avai_animation_states.sort()
	position = sprite_data.offset_global
	position.y += _vertical_offset
	position.y *= pow(_vertical_offset_mult, 1/1.5)
	_set_starting_sprite_data_remotely.rpc([sprite_data.mid, _avai_animation_states, sprite_data.offset_global, position.y])

@rpc
func _set_starting_sprite_data_remotely(data: Array):
	sprite_frames = Global.sprites_datas[data[0]].frames
	_avai_animation_states = data[1]
	position = data[2]
	position.y = data[3]

func change_sprite_data(sprite_data: SpriteData) -> void:
	assert (sprite_data != null)
	sprite_frames = sprite_data.frames
	_avai_animation_states = sprite_data.animation_states
	
	var net_scale: Vector3 = _starting_sprite_width_frontally_sideways_height * _character_width_frontally_sideways_height * sprite_data.width_frontally_sideways_height
	_scale = net_scale

func _play_handled(animation_state: Enum.AnimationState, direction: Enum.Dir, custom_speed: float = 1.0, from_end: bool = false) -> void:
	if _width_asimetrically_altered:
		match direction:
			Enum.Dir.UP, Enum.Dir.DOWN:
				scale.x = _scale[0]
			Enum.Dir.LEFT, Enum.Dir.RIGHT:
				scale.x = _scale[1]
	
	if _avai_animation_states.find(animation_state) != -1:
		super.play(get_animation_name(animation_state, direction), custom_speed, from_end)
	elif animation_state == Enum.AnimationState.WALK and _avai_animation_states.find(Enum.AnimationState.JOG) != -1:
		super.play(get_animation_name(Enum.AnimationState.JOG, direction), custom_speed, from_end)
	elif animation_state == Enum.AnimationState.JOG and _avai_animation_states.find(Enum.AnimationState.WALK) != -1:		
		super.play(get_animation_name(Enum.AnimationState.WALK, direction), custom_speed, from_end)
	else:
		super.play(get_animation_name(Enum.AnimationState.IDLE, direction), custom_speed, from_end)

static func get_animation_name(animation_state: Enum.AnimationState, direction: Enum.Dir) -> StringName:
	match [animation_state, direction]:
		[Enum.AnimationState.IDLE, Enum.Dir.LEFT]:  return Keys.IDLE_LEFT
		[Enum.AnimationState.IDLE, Enum.Dir.RIGHT]: return Keys.IDLE_RIGHT
		[Enum.AnimationState.IDLE, Enum.Dir.DOWN]:  return Keys.IDLE_DOWN
		[Enum.AnimationState.IDLE, Enum.Dir.UP]:    return Keys.IDLE_UP
		[Enum.AnimationState.WALK, Enum.Dir.LEFT]:  return Keys.WALK_LEFT
		[Enum.AnimationState.WALK, Enum.Dir.RIGHT]: return Keys.WALK_RIGHT
		[Enum.AnimationState.WALK, Enum.Dir.DOWN]:  return Keys.WALK_DOWN
		[Enum.AnimationState.WALK, Enum.Dir.UP]:    return Keys.WALK_UP
		[Enum.AnimationState.JOG, Enum.Dir.LEFT]:   return Keys.JOG_LEFT
		[Enum.AnimationState.JOG, Enum.Dir.RIGHT]:  return Keys.JOG_RIGHT
		[Enum.AnimationState.JOG, Enum.Dir.DOWN]:   return Keys.JOG_DOWN
		[Enum.AnimationState.JOG, Enum.Dir.UP]:     return Keys.JOG_UP
		#meter mas animationstates si hace falta, simplemente no usarlos si no se usan en el caso default
		#ejemplo EXTRA1, EXTRA2
	push_error("couldn't match %d %d" % [animation_state, direction])
	return &""
