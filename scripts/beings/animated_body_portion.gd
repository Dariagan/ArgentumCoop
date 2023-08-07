extends AnimatedSprite2D
class_name AnimatedBodyPortion

var _animation_states: Array[StringName]

#_character_scale vector: frontal width, side width, and height
var _character_scale: Vector3 = Vector3.ONE: 
	set(value):
		if value != Vector3.ONE:
			_character_scale = value
			_character_scale_altered = true
		else:
			_character_scale = value
			_character_scale_altered = false

var _character_scale_altered: bool = false

# don't assign a character_scale if the node isn't meant to stretch
func _init(sprite_data: SpriteData = null, character_scale: Vector3 = Vector3.ONE) -> void:
	set_sprite_data(sprite_data)
	self.set("_character_scale", character_scale)

func set_sprite_data(sprite_data: SpriteData) -> void:
	assert (sprite_data != null)
	sprite_frames = sprite_data.frames
	_animation_states = sprite_data.animation_states
	position = sprite_data.offset_global

# DON'T CALL "PLAY" BY ACCIDENT
func _play(animation_name: String = "", custom_speed: float = 1.0, from_end: bool = false) -> void:
	var split_string: PackedStringArray = animation_name.split("_", false, 1)
	var animation_state: StringName = split_string[0]
	var animation_direction: StringName = split_string[1]
	
	if _character_scale_altered:
		match animation_direction:
			&"up", &"down":
				super.apply_scale(Vector2(_character_scale[0], _character_scale[2]))
			&"left", &"right":
				super.apply_scale(Vector2(_character_scale[1], _character_scale[2]))
	
	if animation_state in _animation_states:
		super.play(animation_name, custom_speed, from_end)
	else:	
		super.play(animation_name.replace(animation_state, "idle"), custom_speed, from_end)
	
