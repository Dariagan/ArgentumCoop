extends Faction
class_name PlayerFaction

#type id


var members: Array[Being] = []
	
func _init() -> void:
	color = Color.PALE_GOLDENROD
	id = &"player"
	instance_id = &"player"
	pass
	

func _serialize() -> Dictionary:
	return {}
	
func _deserialize():
	pass
