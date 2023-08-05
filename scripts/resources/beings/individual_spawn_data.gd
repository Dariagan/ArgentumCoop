extends Resource
class_name IndividualBeingSpawnData

@export var health_multiplier: float = 1

@export var level: int = 1

@export var race: BasicRace

@export var size_multiplier: Vector2 = Vector2(1, 1) #width, height

@export var chosen_head_sprite: int
@export var chosen_body_sprite: int = 0

@export var chosen_extra_sprites: Array[int] = []

@export var klass: Class

@export var starting_inventory: Dictionary

