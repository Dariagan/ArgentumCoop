extends Resource
class_name BodyWearData

@export var wear_stats: WearStats

# determines compatibility with character attempting to wear it
@export var tall: bool
@export var male: bool

# graphical
@export var head_offset_x: int = 0
@export var head_offset_y: int = 0

@export var animation: SpriteFrames
