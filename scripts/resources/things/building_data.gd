extends Resource
class_name BuildingDAta

@export var name: String
@export var texture: Texture2D
@export var unbuiltTexture: Texture2D
@export_range(1, 7) var width: int = 1
@export_range(0, 7) var height: int = 1
@export var resourcesRequired: Dictionary
@export var workRequired: int
@export var isRestingSpot: bool = false
@export var isNavigable: bool = false
@export var recipes: Array[RecipeData]
