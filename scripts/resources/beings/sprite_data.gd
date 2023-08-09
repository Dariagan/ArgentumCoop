extends Resource
class_name SpriteData

#sprite node name
@export var name: StringName

@export var id: String

@export var frames: SpriteFrames
@export var sex: Enums.Sex = Enums.Sex.ANY
@export var animation_states: Array[StringName] = [&"idle"]

@export var offset_global: Vector2 = Vector2.ZERO
@export var offset_down: Vector2 = Vector2.ZERO
@export var offset_up: Vector2 = Vector2.ZERO
@export var offset_sideways: Vector2 = Vector2.ZERO



@export var width_frontally_sideways_height: Vector3 = Vector3.ONE
