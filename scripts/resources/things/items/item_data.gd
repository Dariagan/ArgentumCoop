extends Resource
class_name ItemData

@export var id: String
@export var name: String
@export_multiline var description: String
@export var texture: Texture2D
@export_range(0,50) var weight: float
@export_range(0,50) var encumberance: float #should turn to 0 when worn
