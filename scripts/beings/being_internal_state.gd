extends Node# es Node para que sea fácilmente debuggeable desde Remote,y así el state interactúa más fácilmente con el parent Being
# y, puede meterle nuevos nodos 
class_name BeingInternalState 
#para agregar más funcionalidad/estado, addchildear nodos hijos de estado
# VA A HABER Q VER COMO SYNCEAR EN MP

var _carried_weight: int = 0
var _faction: Faction

var _sex: Enums.Sex
var _race: BasicRace #mover estos dos a una clase Characterization guardada en otro lado?
var _klass: Klass = null #mover estos dos a una clase Characterization guardada en otro lado?

var _body: HarmableBody #contains health state for each _body part
var _inventory: InventoryData = null# TODO USAR EL PLUGIN INVENTORYSYSTEM?
var skills_data = null

var _beingkind: BeingKind = null

#necesario para que funciona  serialize()
func construct_for_posterior_serialization(sex: Enums.Sex, race: BasicRace, faction: Faction, body: HarmableBody, klass: Klass, beingkind: BeingKind):
	self._sex = sex; self._race = race; self._faction = faction; self._body = body; self._klass= klass; self._beingkind = beingkind

@rpc("call_local")
func construct_from_seri(serialized_self: Dictionary) -> void:
	_sex = serialized_self[BeingStatePreIniter.KCONS.SEX]
	
	_faction = GameData.factions[serialized_self[BeingStatePreIniter.KCONS.FACTION]]
	#_body = HarmableBody.new(serialized_self["_body"])
	
	_race = GlobalData.races[serialized_self[BeingStatePreIniter.KCONS.RACE]]
	if _race is ControllableRace:
		_klass = GlobalData.klasses[serialized_self[BeingStatePreIniter.KCONS.KLASS]]

func get_max_speed() -> float:
	var max_speed: float =  7 * _race.combat_multipliers.speed 
	if _klass:
		max_speed *= _klass.combat_multipliers.speed
	return max_speed

func serialize() -> Dictionary:
	assert(_sex and _race and _faction)
	var data: Dictionary =  {
		BeingStatePreIniter.KCONS.SEX: _sex,
		BeingStatePreIniter.KCONS.RACE: _race.id,
		#"_body": _body.serialize(),
		BeingStatePreIniter.KCONS.FACTION: _faction.instance_id,
		"inv": {}#.serialize()
	}
	if _klass: data[BeingStatePreIniter.KCONS.KLASS] = _klass.id
	return data
