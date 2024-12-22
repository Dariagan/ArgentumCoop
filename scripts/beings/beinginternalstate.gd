extends Node# es Node para que sea fácilmente debuggeable desde Remote,y así el state interactúa más fácilmente con el parent Being
# y, puede meterle nuevos nodos 
class_name BeingInternalState 
#para agregar más funcionalidad/estado, addchildear nodos hijos de estado
# VA A HABER Q VER COMO SYNCEAR EN MP

var mcarried_weight: int = 0
var mfaction: Faction

var msex: Enums.Sex
var mrace: BasicRace #mover estos dos a una clase Characterization guardada en otro lado?
var mklass: Klass = null #mover estos dos a una clase Characterization guardada en otro lado?

var mbody: HarmableBody #contains health state for each body part
var minventory: InventoryData = null# TODO USAR EL PLUGIN INVENTORYSYSTEM?
var mskills_data = null

var mbeing_gen_template: BeingGenTemplate = null 
# https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#class-globalscope-method-weakref
var mfollowers: Array[Being] = []
var mmaster: Being = null
#necesario para que funciona  serialize()
func construct_for_posterior_serialization(psex: Enums.Sex, prace: BasicRace, pfaction: Faction, pbody: HarmableBody, pklass: Klass, pbeing_gen_template: BeingGenTemplate):
	self.msex = psex; self.mrace = prace; self.mfaction = pfaction; self.mbody = pbody; self.mklass= pklass; self.mbeing_gen_template = pbeing_gen_template

@rpc("call_local")
func construct_from_seri(pserialized_self: Dictionary) -> void:
	msex = pserialized_self[Keys.SEX]
	
	mfaction = GameData.factions[pserialized_self[Keys.FACTION]]
	#mbody = HarmableBody.new(pserialized_self[Keys.BODY])
	
	mrace = Global.races[pserialized_self[Keys.RACE]]
	if mrace is ControllableRace:
		mklass = Global.klasses[pserialized_self[Keys.KLASS]]

func get_max_speed() -> float:
	var max_speed: float =  400 * mrace.mcombat_multipliers.speed 
	if mklass:
		max_speed *= mklass.mcombat_multipliers.speed
	return max_speed

func serialize() -> Dictionary:
	assert(msex and mrace and mfaction)
	var data: Dictionary =  {
		Keys.SEX: msex,
		Keys.RACE: mrace.mid,
		#"mbody": mbody.serialize(),
		Keys.FACTION: mfaction.minstance_id,
		"inv": {}#.serialize()
	}
	if mklass: data[Keys.KLASS] = mklass.mid
	return data
