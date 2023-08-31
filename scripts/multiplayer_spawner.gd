extends MultiplayerSpawner


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	for path in GlobalData.spawnable_scenes:
		add_spawnable_scene(path)
