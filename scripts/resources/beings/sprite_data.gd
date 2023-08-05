extends Resource
class_name SpriteData

#animated body portion name (Head, Body, Leg1, Leg2)
@export var name: String
@export var frames: SpriteFrames
@export var sex: Enums.Sex = Enums.Sex.ANY

@export var offset_down: Vector2 = Vector2.ZERO
@export var offset_up: Vector2 = Vector2.ZERO
@export var offset_sideways: Vector2 = Vector2.ZERO
