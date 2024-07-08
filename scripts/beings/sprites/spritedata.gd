extends Resource
class_name SpriteData

var id: StringName

#sprite node name
@export var name: StringName


@export var frames: SpriteFrames

@export var sex: Enums.Sex = Enums.Sex.ANY
@export var animation_states: Array[Enums.AnimationState] = [Enums.AnimationState.IDLE]

@export var offset_global: Vector2 = Vector2.ZERO
@export var offset_looking_down: Vector2 = Vector2.ZERO
@export var offset_looking_up: Vector2 = Vector2.ZERO
@export var offset_looking_sideways: Vector2 = Vector2.ZERO

@export var simmetrical_sideways: bool = false

@export var width_frontally_sideways_height: Vector3 = Vector3.ONE
