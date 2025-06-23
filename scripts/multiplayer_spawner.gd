extends MultiplayerSpawner


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	for scene: PackedScene in Global.packed_scenes.values():
		add_spawnable_scene(scene.resource_path)
