extends Node

const item_data_directories: Array[String] = []#["res://resources/things/items/", "res://resources/things/items/wear/body/"]
const recipes_directories: Array[String] = []
const building_data_directories: Array[String] = []
const controllable_races_directories: Array[String] = ["res://resources/beings/controllable/races/"]
const uncontrollable_races_directories: Array[String] = ["res://resources/beings/uncontrollable/races/"]
const classes_directories: Array[String] = ["res://resources/beings/controllable/classes/"]

var item_data: Dictionary
var recipe_data: Dictionary
var building_data: Dictionary
var controllable_races: Dictionary
var uncontrollable_races: Dictionary
var classes: Dictionary

func _init() -> void:
	item_data = _index_all_found_resources(item_data_directories)
	recipe_data = _index_all_found_resources(recipes_directories)
	building_data = _index_all_found_resources(building_data_directories)
	controllable_races = _index_all_found_resources(controllable_races_directories)
	uncontrollable_races = _index_all_found_resources(uncontrollable_races_directories)
	classes = _index_all_found_resources(classes_directories)

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
						print("File %s at %s couldn't be loaded as a resource" % [file_name, directory])
					
				file_name = dir_access.get_next()
		else:
			print("Couldn't open directory %s" % [directory])
	
	return table

