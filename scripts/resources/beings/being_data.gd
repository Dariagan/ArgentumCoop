extends Resource
class_name BeingPersonalData 

var health: int #hacer componente, o no no sé
var max_speed: int
var carried_weight: int = 0
var faction: Faction

var race: BasicRace
var body: HarmeableBody

var inventory_data: InventoryData


func _init(data: Dictionary = {}) -> void:
	
	race = GlobalData.races[data["race"]]
	faction = GameData.factions[data["faction"]]
	
	
	
	
	
