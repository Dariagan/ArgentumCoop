extends Resource
#data 
class_name BeingPersonalData 

var health: int #hacer componente, o no no sé
var max_speed: int
var carried_weight: int
var faction: Faction

var race: BasicRace
var body: Body



var inventory_data: InventoryData


func _init() -> void:
	body = race.body.duplicate(true)
