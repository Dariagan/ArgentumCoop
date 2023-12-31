extends Resource
class_name BuildingData

@export var id: String
@export var name: String
@export var built_tile: Tile
@export var unbuilt_tile: Tile
@export_range(1, 7) var width: int = 1
@export_range(1, 7) var length: int = 1
@export var resources_required: Dictionary #key: id del item, value: cantidad
@export var work_required: int
@export var is_resting_spot: bool = false
@export var is_navigable: bool = false

# for workbenches
@export var recipes: Array[RecipeData]
