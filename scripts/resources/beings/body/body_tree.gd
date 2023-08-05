extends Resource
class_name BodyTree

@export var body_part: BodyPart
@export var coverage: float
@export var character_side: int

@export var children: Array[BodyTree]

# not inherited by children, only local to the body part
# body groups the part belongs to (arm, leg, shoulder, etc)
@export var body_groups: Array[String]

