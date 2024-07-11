extends Object
class_name Global
# PRESIONA F1 PARA IMPRIMIR TU POSICIÓN ACTUAL EN EL TILEMAP POR CONSOLA!
static var username: String

static var item_data: Dictionary

static var sprites_datas: Dictionary

static var recipe_data: Dictionary
static var building_data: Dictionary

static var races: Dictionary
static var controllable_races: Dictionary
static var uncontrollable_races: Dictionary
static var klasses: Dictionary
static var being_gen_templates: Dictionary

static var tilesdict: Dictionary

static var tile_selections: Dictionary

static var spawnable_scenes: Array[String]
static var music: Dictionary = {}
static var music_peace_order: Dictionary
static var taunt_sounds: Dictionary
static var shaders: Dictionary

#causa error al descomentar (ya está cargado)
#var tile_set: TileSet = preload("res://resource_instances/tiling/tiles/tile_set.tres")

func _init() -> void:
	Engine.register_singleton(&"Global", self)
	const item_data_dirs: Array[String] = []#["res://resource_instances/things/items/", "res://resource_instances/things/items/wear/body/"]
	const sprites_datas_dirs: Array[String] = ["res://resource_instances/beings/sprites/spritesdatas/"]
	const recipes_dirs: Array[String] = []
	const building_data_dirs: Array[String] = []
	const controllable_races_dirs: Array[String] = ["res://resource_instances/beings/races/controllable/"]
	const uncontrollable_races_dirs: Array[String] = ["res://resource_instances/beings/races/uncontrollable/"]
	const tile_selections_dirs: Array[String] =["res://resource_instances/tiling/tile_selections/"]
	const klasses_dirs: Array[String] = ["res://resource_instances/beings/klasses/"]
	const tiles_dirs: Array[String] = ["res://resource_instances/tiling/tiles/terrain/", "res://resource_instances/tiling/tiles/structures/"]
	#const spawnable_scenes_dirs: Array[String] = ["res://scenes/tiles/"]
	const being_gen_templates_dirs: Array[String] = ["res://resource_instances/beings/being_gen_templates/"]
	const taunt_dirs: Array[String] = ["res://assets/sound/taunts/"]
	const music_peace_order_dirs: Array[String] = ["res://assets/sound/music/ingame/peace/order/"]
	const shaders_dirs: Array[String] = ["res://resource_instances/shaders/"]
	
	item_data = _index_all_found_resource_instances(item_data_dirs, true)
	sprites_datas = _index_all_found_resource_instances(sprites_datas_dirs, true)
	sprites_datas.make_read_only()
	recipe_data = _index_all_found_resource_instances(recipes_dirs, true)
	building_data = _index_all_found_resource_instances(building_data_dirs, true)
	controllable_races = _index_all_found_resource_instances(controllable_races_dirs, true)
	uncontrollable_races = _index_all_found_resource_instances(uncontrollable_races_dirs, true)
	races.merge(uncontrollable_races, true); races.merge(controllable_races, true); races.make_read_only()
	
	tile_selections = _index_all_found_resource_instances(tile_selections_dirs, false); tile_selections.make_read_only()
	klasses = _index_all_found_resource_instances(klasses_dirs, true); klasses.make_read_only()
	tilesdict = _index_all_found_resource_instances(tiles_dirs, true); tilesdict.make_read_only()
	
	being_gen_templates = _index_all_found_resource_instances(being_gen_templates_dirs, true); being_gen_templates.make_read_only()
	
	taunt_sounds = _index_all_found_resource_instances(taunt_dirs, false); taunt_sounds.make_read_only()
	
	music_peace_order = _index_all_found_resource_instances(music_peace_order_dirs, true); music_peace_order.make_read_only()
	
	music[Keys.PEACE_ORDER] = music_peace_order
	
	shaders = _index_all_found_resource_instances(shaders_dirs, true); shaders.make_read_only()

	#spawnable_scenes = _list_all_spawnable_scenes(spawnable_scenes_dirs)

static func allowed_file_extension(file_name: String) -> bool:
	const blacklisted_extensions: Array[String] = [".import", ".gdshader"]
	for ex in blacklisted_extensions:
		if file_name.ends_with(ex):
			return false
	return true

#TODO CHEQUEAR COLLISION DE KEYS ENCONTRADAS ANTES DE METER AL DICT (PUSHEAR UN ERROR)
static func _index_all_found_resource_instances(dirs: Array[String], check_subfolders: bool, use_safe_loader: bool = false) -> Dictionary:
	var dir_access: DirAccess; var table: Dictionary = {}
	for directory in dirs:
		if not directory.ends_with("/"): directory += "/"
		dir_access = DirAccess.open(directory)
		if dir_access:
			dir_access.list_dir_begin()
			var file_name = dir_access.get_next()
			while file_name != "":
				if !dir_access.current_is_dir():
					if allowed_file_extension(file_name):
						var resource
						if not use_safe_loader:
							resource = ResourceLoader.load(directory + file_name)
						else:#NO USAR SI ESTÁ EN .res/ EL DIRECTORIO, NO HACE FALTA
							resource = SafeResourceLoader.load(directory + file_name)
						
						if resource:
							var id: StringName = file_name.get_basename()
							if file_name.ends_with(".tres") and resource is not ShaderMaterial: resource.id = id
							if table.has(id):
								push_warning("a resource with id=%s is already present in target dict"%[resource.id])
								resource = null
							if resource:
								if resource.has_method(&"validate") && not resource.validate():
									printerr("resource %s doesn't meet its validation condition"%[file_name])
									resource = null
							if resource:
								table[id] = resource
						else:
							printerr("%s%s was NOT added into its target dictionary" % [directory, file_name])
								
				elif check_subfolders:
					var subdict: Dictionary = _index_all_found_resource_instances([directory+file_name+"/"], true)
					for key in subdict:
						assert(not table.has(key))
						table[key] = subdict[key]
					
				file_name = dir_access.get_next()
		else:
			printerr("Couldn't open directory %s" % [directory])
	return table

func _list_all_spawnable_scenes(dirs: Array[String]) -> Array[String]:
	var dir_access: DirAccess
	var found_scenes: Array[String] = []
	
	for directory in dirs:
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
