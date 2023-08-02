extends Resource
class_name BuildingData

@export var id: String
@export var name: String
@export var texture: Texture2D
@export var unbuilt_texture: Texture2D
@export_range(1, 7) var width: int = 1
@export_range(1, 7) var length: int = 1
@export var resources_required: Dictionary
@export var work_required: int
@export var is_resting_spot: bool = false
@export var is_navigable: bool = false

# for workbenches
@export var recipes: Array[RecipeData]
