# singleton
extends Object
class_name GameData

func _init() -> void:
	Engine.register_singleton(&"GameData", self)


#clearear al salir de la partida
static var factions: Dictionary = {
	&"player": PlayerFaction.new(),# en el caso de cargar una savefile, va a haber q agregarlo diferentemente
	&"wild": WildFaction.new(),
}
