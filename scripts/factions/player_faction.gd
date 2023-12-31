extends Faction
class_name PlayerFaction

var id: String = "player"
var instance_id: String = "player"

var members: Array[Being] = []
	
func _init() -> void:
	pass
	

func _serialize() -> Dictionary:
	return {}
	
func _deserialize():
	pass
