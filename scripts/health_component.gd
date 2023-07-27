extends Node2D
class_name HealthComponent

@export var max_health: int = 100
var health : int = max_health

func _ready() -> void:
	health = max_health


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
