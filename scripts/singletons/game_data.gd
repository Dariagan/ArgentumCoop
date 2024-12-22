# singleton
extends Object
class_name GameData

func _init() -> void:
	Engine.register_singleton(&"GameData", self)


#clearear al salir de la partida
static var factions: Dictionary[StringName, Faction] = {
	Keys.PLAYER_FACTION_INSTANCE: PlayerFaction.new(),# en el caso de cargar una savefile, va a haber q agregarlo diferentemente
	Keys.WILD_FACTION_INSTANCE: WildFaction.new(),
}
