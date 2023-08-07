extends Resource
class_name BodyPart


@export var name: String
@export var max_hit_points: float

@export var bleed_rate: float = 0

@export var frostbite_vulnerability: float = 0
@export var external: bool

@export var causes_pain: bool = true

@export var replaceable: bool = false

@export var bodily_function: Array

var injuries: Array # tupla float, attack type

var health = max_hit_points

var is_destroyed: bool = false

signal destroyed


