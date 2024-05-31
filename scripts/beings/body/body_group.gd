extends Resource
# meant for grouping bodyparts together for assigning a whole limb to a hitbox, or assigning what body group does a wearable piece protect
class_name BodyGroup

var _name: String

var found_body_parts: Array[BodyPart]

func _init(name: String = "") -> void:
	_name = name
