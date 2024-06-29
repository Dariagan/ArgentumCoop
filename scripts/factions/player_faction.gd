extends Faction
class_name PlayerFaction

#type id
var id: StringName = &"player"
var instance_id: StringName = &"player"

var members: Array[Being] = []
	
func _init() -> void:
	name_label_color = Color.PALE_GOLDENROD
	pass
	

func _serialize() -> Dictionary:
	return {}
	
func _deserialize():
	pass
