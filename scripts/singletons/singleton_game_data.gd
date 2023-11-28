# singleton
extends Node


#clearear al salir de la partida
var factions: Dictionary = {
	"player" = PlayerFaction.new()# en el caso de cargar una savefile, va a haber q agregarlo diferentemente
}

