extends Faction
class_name PlayerFaction

#type id


var members: Array[Being] = []
	
func _init() -> void:
	instance_id = &"player"
	color = Color.BISQUE
	pass
	

func _serialize() -> Dictionary:
	return {}
	
func _deserialize():
	pass
