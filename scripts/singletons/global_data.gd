extends Node

var username: String

const item_data_directories: Array[String] = []#["res://resources/things/items/", "res://resources/things/items/wear/body/"]
const sprites_datas_directories: Array[String] = ["res://resources/beings/controllable/sprites/head/", "res://resources/beings/controllable/sprites/body/", "res://resources/beings/uncontrollable/sprites/body/"]
const recipes_directories: Array[String] = []
const building_data_directories: Array[String] = []
const controllable_races_directories: Array[String] = ["res://resources/beings/controllable/races/"]
const uncontrollable_races_directories: Array[String] = ["res://resources/beings/uncontrollable/races/"]
const races_directories = controllable_races_directories + uncontrollable_races_directories
const classes_directories: Array[String] = ["res://resources/beings/controllable/classes/"]
const tiles_directories: Array[String] = ["res://resources/world/terrain/", "res://resources/world/buildings/", "res://resources/world/buildings/dungeons/"]
const spawnable_scenes_directories: Array[String] = ["res://scenes/world/terrain/", "res://scenes/world/buildings/"]

var item_data: Dictionary

var sprites_datas: Dictionary

var recipe_data: Dictionary
var building_data: Dictionary

var races: Dictionary
var controllable_races: Dictionary
var uncontrollable_races: Dictionary
var classes: Dictionary

var tiles: Dictionary

var spawnable_scenes: Array[String]

#causa error al descomentar (ya está cargado)
#var tile_set: TileSet = preload("res://resources/world/tile_set.tres")

func _init() -> void:
	
	item_data = _index_all_found_resources(item_data_directories)
	sprites_datas = _index_all_found_resources(sprites_datas_directories)
	recipe_data = _index_all_found_resources(recipes_directories)
	building_data = _index_all_found_resources(building_data_directories)
	controllable_races = _index_all_found_resources(controllable_races_directories)
	uncontrollable_races = _index_all_found_resources(uncontrollable_races_directories)
	races.merge(uncontrollable_races, true); races.merge(controllable_races, true)
	
	classes = _index_all_found_resources(classes_directories)
	tiles = _index_all_found_resources(tiles_directories)
	
	spawnable_scenes = _list_all_spawnable_scenes(spawnable_scenes_directories)

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

func _list_all_spawnable_scenes(directories: Array[String]) -> Array[String]:
	var dir_access: DirAccess
	var found_scenes: Array[String]
	
	for directory in directories:
		dir_access = DirAccess.open(directory) 
		
		if dir_access:
			dir_access.list_dir_begin()
			var file_name = dir_access.get_next()
			
			while file_name != "":
				
				if !dir_access.current_is_dir() and file_name.ends_with(".tscn"):
					found_scenes.append(directory + file_name)
					
				file_name = dir_access.get_next()
		else:
			print("Couldn't open directory %s" % [directory])
	
	return found_scenes
