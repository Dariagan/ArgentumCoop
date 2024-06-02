extends Node

#region Debugging configuration
@export var ignore_joiners_readiness_on_start: bool = true
@export var insta_start: bool = false
@export var debug: bool = true
@export var debug_walk_mult:float = 3
@export var noclip: bool = true
@export var noclip_speed_mult:float = 600
var default_ip_to_join: String = "localhost"
#endregion Debugging configuration

# PRESIONA F1 PARA IMPRIMIR TU POSICIÓN ACTUAL EN EL TILEMAP POR CONSOLA!

var username: String

const CPP_BUFFER_LENGTH_MINUS_ONE = 32 - 1
const item_data_directories: Array[String] = []#["res://resource_instances/things/items/", "res://resource_instances/things/items/wear/body/"]
const sprites_datas_directories: Array[String] = ["res://resource_instances/beings/sprites/spritesdatas/"]
const recipes_directories: Array[String] = []
const building_data_directories: Array[String] = []
const controllable_races_directories: Array[String] = ["res://resource_instances/beings/races/controllable/"]
const uncontrollable_races_directories: Array[String] = ["res://resource_instances/beings/races/uncontrollable/"]
const tile_selections_directories: Array[String] =["res://resource_instances/tiling/tiles/tile_selections/"]

const klasses_directories: Array[String] = ["res://resource_instances/beings/klasses/"]
const tiles_directories: Array[String] = ["res://resource_instances/tiling/tiles/terrain/", "res://resource_instances/tiling/tiles/buildings/"]
const spawnable_scenes_directories: Array[String] = ["res://scenes/tiling/tiles/terrain/", "res://scenes/tiling/tiles/buildings/"]

const beingkinds_directories: Array[String] = ["res://resource_instances/beings/beingkinds/"]
#TODO hacer un player mods directory

var item_data: Dictionary

var sprites_datas: Dictionary

var recipe_data: Dictionary
var building_data: Dictionary

var races: Dictionary
var controllable_races: Dictionary
var uncontrollable_races: Dictionary
var klasses: Dictionary
var beingkinds: Dictionary

var tiles: Dictionary

var tile_selections: Dictionary

var spawnable_scenes: Array[String]

#causa error al descomentar (ya está cargado)
#var tile_set: TileSet = preload("res://resource_instances/tiling/tiles/tile_set.tres")

func _init() -> void:
	
	item_data = _index_all_found_resource_instances(item_data_directories)
	sprites_datas = _index_all_found_resource_instances(sprites_datas_directories)
	sprites_datas.make_read_only()
	recipe_data = _index_all_found_resource_instances(recipes_directories)
	building_data = _index_all_found_resource_instances(building_data_directories)
	controllable_races = _index_all_found_resource_instances(controllable_races_directories)
	uncontrollable_races = _index_all_found_resource_instances(uncontrollable_races_directories)
	races.merge(uncontrollable_races, true); races.merge(controllable_races, true)
	races.make_read_only()
	
	tile_selections = _index_all_found_resource_instances(tile_selections_directories)
	tile_selections.make_read_only()
	klasses = _index_all_found_resource_instances(klasses_directories)
	klasses.make_read_only()
	tiles = _index_all_found_resource_instances(tiles_directories)
	tiles.make_read_only()
	
	beingkinds = _index_all_found_resource_instances(beingkinds_directories, true)
	
	spawnable_scenes = _list_all_spawnable_scenes(spawnable_scenes_directories)

#TODO CHEQUEAR COLLISION DE KEYS ENCONTRADAS ANTES DE METER AL DICT (PUSHEAR UN ERROR)
func _index_all_found_resource_instances(directories: Array[String], check_subfolders: bool = true, use_safe_loader: bool = false) -> Dictionary:
	var dir_access: DirAccess
	var table: Dictionary = {}
	
	for directory in directories:
		dir_access = DirAccess.open(directory) 
		
		if dir_access:
			dir_access.list_dir_begin()
			var file_name = dir_access.get_next()
			
			while file_name != "":
				
				if !dir_access.current_is_dir():
					var resource
					if not use_safe_loader:
						resource = ResourceLoader.load(directory + file_name)
					else:#NO USAR SI ESTÁ EN .res/ EL DIRECTORIO, NO HACE FALTA
						resource = SafeResourceLoader.load(directory + file_name)
					
					if resource && "id" in resource:
						assert(not table.has(resource.id))
						table[resource.id] = resource
						print("Resource %s%s loaded" % [directory, file_name])
					else:
						printerr("File %s%s couldn't be loaded as a resource" % [directory, file_name])
				elif check_subfolders:
					var subdict: Dictionary = _index_all_found_resource_instances([directory+file_name+"/"])
					for key in subdict:
						assert(not table.has(key))
						table[key] = subdict[key]
					
				file_name = dir_access.get_next()
		else:
			printerr("Couldn't open directory %s" % [directory])
	#table.make_read_only()
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
