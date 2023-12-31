extends Resource
class_name Wear

@export var wear_stats: WearStats

@export var compatible_races: Array[BasicRace]

@export var sex: Enums.Sex

@export var frames: SpriteFrames

@export var equipped_in_node: String

@export var covered_body_groups: Array[String]#Bodygroups
#hacer dictionary y poner de value el multiplier para esa parte?

@export var occupied_layers: Array[Enums.Layer] = [Enums.Layer.BOTTOM]
