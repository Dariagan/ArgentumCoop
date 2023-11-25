extends Node

#region Debugging configuration
@export var ignore_joiners_readiness_on_start: bool = true
@export var insta_start: bool = true
@export var debug: bool = true
@export var debug_walk_mult:float = 3
@export var noclip: bool = true
@export var noclip_speed_mult:float = 600
var default_ip_to_join: String = "localhost"
#endregion Debugging configuration

# PRESIONA F1 PARA IMPRIMIR TU POSICIÓN ACTUAL EN EL TILEMAP POR CONSOLA!

var username: String

const CPP_BUFFER_LENGTH_MINUS_ONE = 32 - 1
const item_data_directories: Array[String] = []#["res://resources/things/items/", "res://resources/things/items/wear/body/"]
const sprites_datas_directories: Array[String] = ["res://resources/beings/sprites/spritesdatas/"]
const recipes_directories: Array[String] = []
const building_data_directories: Array[String] = []
const controllable_races_directories: Array[String] = ["res://resources/beings/races/controllable/"]
const uncontrollable_races_directories: Array[String] = ["res://resources/beings/races/uncontrollable/"]
const tile_selections_directories: Array[String] =["res://resources/world/tile_selections/"]

const classes_directories: Array[String] = ["res://resources/beings/classes/"]
const tiles_directories: Array[String] = ["res://resources/world/terrain/", "res://resources/world/buildings/"]
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

var tile_selections: Dictionary

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
	
	tile_selections = _index_all_found_resources(tile_selections_directories)
	
	classes = _index_all_found_resources(classes_directories)
	tiles = _index_all_found_resources(tiles_directories)
	
	spawnable_scenes = _list_all_spawnable_scenes(spawnable_scenes_directories)

func _index_all_found_resources(directories: Array[String], check_subfolders: bool = true) -> Dictionary:
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
			
					if resource && "id" in resource:
						table[resource.id] = resource
						print("Resource %s%s loaded" % [directory, file_name])
					else:
						printerr("File %s%s couldn't be loaded as a resource" % [directory, file_name])
				elif check_subfolders:
					table.merge(_index_all_found_resources([directory+file_name+"/"]), true)
					
				file_name = dir_access.get_next()
		else:
			printerr("Couldn't open directory %s" % [directory])
	
	return table

func _list_all_spawnable_scenes(directories: Array[String]) -> Array[String]:
	var dir_access: DirAccess
	var found_scenes: Array[String] = []
	
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
