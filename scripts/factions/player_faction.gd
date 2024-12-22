extends Faction
class_name PlayerFaction

#type id


var mmembers: Array[Being] = []
	
func _init() -> void:
	minstance_id = Keys.PLAYER_FACTION_INSTANCE
	mcolor = Color.BISQUE
	pass
	

func _serialize() -> Dictionary:
	return {}
	
func _deserialize():
	pass
