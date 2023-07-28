extends Node2D
class_name HealthComponent

signal killed 

@export var max_health: int = 100
var health : int = max_health

func _ready() -> void:
	health = max_health

func damage(attack: Attack):
	health -= attack.damage

	if health <= 0:
		health = 0
		emit_signal("killed")
