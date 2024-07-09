extends Node# es Node para que sea fácilmente debuggeable desde Remote,y así el state interactúa más fácilmente con el parent Being
# y, puede meterle nuevos nodos 
class_name BeingInternalState 
#para agregar más funcionalidad/estado, addchildear nodos hijos de estado
# VA A HABER Q VER COMO SYNCEAR EN MP

var _carried_weight: int = 0
var faction: Faction

var sex: Enums.Sex
var race: BasicRace #mover estos dos a una clase Characterization guardada en otro lado?
var klass: Klass = null #mover estos dos a una clase Characterization guardada en otro lado?

var body: HarmableBody #contains health state for each body part
var inventory: InventoryData = null# TODO USAR EL PLUGIN INVENTORYSYSTEM?
var skills_data = null

var being_gen_template: BeingGenTemplate = null 
# https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#class-globalscope-method-weakref
var followers: Array[Being] = []
var master: Being = null
#necesario para que funciona  serialize()
func construct_for_posterior_serialization(sex: Enums.Sex, race: BasicRace, faction: Faction, body: HarmableBody, klass: Klass, being_gen_template: BeingGenTemplate):
	self.sex = sex; self.race = race; self.faction = faction; self.body = body; self.klass= klass; self.being_gen_template = being_gen_template

@rpc("call_local")
func construct_from_seri(serialized_self: Dictionary) -> void:
	sex = serialized_self[Keys.SEX]
	
	faction = GameData.factions[serialized_self[Keys.FACTION]]
	#body = HarmableBody.new(serialized_self["body"])
	
	race = Global.races[serialized_self[Keys.RACE]]
	if race is ControllableRace:
		klass = Global.klasses[serialized_self[Keys.KLASS]]

func get_max_speed() -> float:
	var max_speed: float =  400 * race.combat_multipliers.speed 
	if klass:
		max_speed *= klass.combat_multipliers.speed
	return max_speed

func serialize() -> Dictionary:
	assert(sex and race and faction)
	var data: Dictionary =  {
		Keys.SEX: sex,
		Keys.RACE: race.id,
		#"body": body.serialize(),
		Keys.FACTION: faction.instance_id,
		"inv": {}#.serialize()
	}
	if klass: data[Keys.KLASS] = klass.id
	return data
