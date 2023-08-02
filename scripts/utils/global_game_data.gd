extends Node

const item_data_directories: Array[String] = ["res://resources/items/", "res://resources/items/wear/body/"]
const recipes_directories: Array[String] = []
const building_data_directories: Array[String] = []
const humanoid_races_directories: Array[String] = ["res://resources/character/races/humanoid/"]
const animal_races_directories: Array[String] = []

var item_data: Dictionary
var recipe_data: Dictionary
var building_data: Dictionary
var humanoid_races: Dictionary
var animal_races: Dictionary

func _init() -> void:
	item_data = _index_all_found_resources(item_data_directories)
	recipe_data = _index_all_found_resources(recipes_directories)
	building_data = _index_all_found_resources(building_data_directories)
	humanoid_races = _index_all_found_resources(humanoid_races_directories)
	animal_races = _index_all_found_resources(animal_races_directories)

func _index_all_found_resources(directories: Array[String]) -> Dictionary:
	var dir_access: DirAccess
	var table: Dictionary = {}
	
	for directory in directories:
		dir_access = DirAccess.open(directory) 
		
		if dir_access:
			dir_access.list_dir_begin()
			var file_name = dir_access.get_next()
			
			while file_name != "":
				
				if !dir_access.current_is_dir():
					var resource = ResourceLoader.load(directory + file_name)
			
					if resource:
						table[resource.id] = resource
						print("Resource %s at %s loaded" % [file_name, directory])
					else:
						print("File %s at couldn't be loaded as a resource" % [file_name, directory])
					
				file_name = dir_access.get_next()
		else:
			push_error("Couldn't open directory %s" % [directory])
	
	return table

