extends Node# es Node para que sea fácilmente debuggeable desde Remote,y así el state interactúa más fácilmente con el parent Being
class_name BeingInternalState 
#para agregar más funcionalidad/estado, addchildear nodos hijos de estado

var carried_weight: int = 0
var faction: Faction

var sex: Enums.Sex
var race: BasicRace #mover estos dos a una clase Characterization guardada en otro lado?
var klass: Klass = null #mover estos dos a una clase Characterization guardada en otro lado?

var body: HarmableBody #contains health state for each body part
var inventory: InventoryData = null# TODO USAR EL PLUGIN INVENTORYSYSTEM?
var skills_data = null

#necesario para que funciona  serialize()
func construct_locally(sex: Enums.Sex, race:BasicRace, faction: Faction, body: HarmableBody, klass: Klass = null):
	self.sex = sex; self.race = race; self.faction = faction; self.body = body; self.klass= klass

# to be called only from RPC
func construct(data: Dictionary) -> void:
	sex = data["sex"]
	
	faction = GameData.factions[data["fac"]]
	#body = HarmableBody.new(data["body"])
	
	race = GlobalData.races[data["race"]]
	if race is ControllableRace:
		klass = GlobalData.klasses[data["klass"]]

func get_max_speed() -> float:
	var max_speed: float =  7 * race.combat_multipliers.speed 
	if klass:
		max_speed *= klass.combat_multipliers.speed
	return max_speed

func serialize() -> Dictionary:
	var data: Dictionary =  {
		"sex": sex,
		"race": race.id,
		#"body": body.serialize(),
		"fac": faction.instance_id,
		"inv": {}#.serialize()
	}
	if klass: data["klass"] = klass.id
	return data
