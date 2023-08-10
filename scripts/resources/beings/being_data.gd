extends Resource
class_name BeingPersonalData 

var health: int #hacer componente, o no no sé

var carried_weight: int = 0
var faction: Faction

var race: BasicRace
var klass: Class
var body: HarmableBody

var inventory_data: InventoryData


func _init(data: Dictionary = {}) -> void:
	
	race = GlobalData.races[data["race"]]
	if race is ControllableRace:
		klass = GlobalData.classes[data["klass"]]
	
	faction = GameData.factions[data["faction"]]
	
func get_max_speed() -> float:
	var max_speed: float =  3 * race.combat_multipliers.max_speed 
	if klass:
		max_speed *= klass.combat_multipliers.max_speed
	return max_speed
	
