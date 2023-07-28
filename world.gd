extends Node2D
class_name GameWorld

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.



func _process(delta: float) -> void:
	pass


@onready var tilemap: TileMap = $OverWorldTileMap

func spawn_player(character: Character):
	if tilemap.has_method("spawn_player"):
		tilemap.spawn_player(character)
