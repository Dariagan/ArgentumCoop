extends Node
class_name TypesData

enum Sex { ANY, MALE, FEMALE }

@export var item_data_directories: Array[String]
@export var recipes_directories: Array[String]
@export var building_data_directories: Array[String]
@export var races_directories: Array[String]

var item_data: Dictionary
var recipes: Dictionary
var building_data: Dictionary
var races: Dictionary

func _init() -> void:
	item_data = _index_all_found_resources(item_data_directories, ItemData)
	recipes = _index_all_found_resources(recipes_directories, RecipeData)
	building_data = _index_all_found_resources(building_data_directories, BuildingData)
	races = _index_all_found_resources(races_directories, Race)

func _index_all_found_resources(directories: Array[String], type_to_check: Variant) -> Dictionary:
	var dirAccess: DirAccess
	var table: Dictionary = {}
	for directory in directories:
		dirAccess = DirAccess.open(directory) 
		if dirAccess:
			var file_name = dirAccess.get_next()
			while file_name != "":
				var resource = ResourceLoader.load(file_name)
				
				if resource:
					if resource.is_instance_of(type_to_check):
						table[resource.id] = resource
					else:
						print("Resource at %s not loaded, recourse is not of expected type %s" % [file_name, str(type_to_check)])
				else:
					print("File %s couldn't be loaded as a resource" % [file_name])
					
				file_name = dirAccess.get_next()
				
			dirAccess.close()
		else:
			push_error("Couldn't open directory %s when loading %s" % [directory, str(type_to_check)])
	
	return table
