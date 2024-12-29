extends Resource
class_name SpriteData

var mid: StringName

#sprite node name
@export var name: StringName


@export var frames: SpriteFrames

@export var sex: Enum.Sex = Enum.Sex.ANY
@export var animation_states: Array[Enum.AnimationState] = [Enum.AnimationState.IDLE]

@export var offset_global: Vector2 = Vector2.ZERO
@export var offset_looking_down: Vector2 = Vector2.ZERO
@export var offset_looking_up: Vector2 = Vector2.ZERO
@export var offset_looking_sideways: Vector2 = Vector2.ZERO

@export var simmetrical_sideways: bool = false

@export var width_frontally_sideways_height: Vector3 = Vector3.ONE
