extends Node2D
class_name GameWorld

@onready var tilemap: TileMap = $OverWorldTileMap

signal am_ready

func _ready() -> void:
	am_ready.emit()


func spawn_player(character: Character):
	if tilemap.has_method("spawn_player"):
		tilemap.spawn_player(character)
	
