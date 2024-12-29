extends StaticBody2D
class_name Portal
#subject to changes

#assigned by user, not random
var mid: StringName
@onready var mtrigger_area: Area2D = $Area2D
@onready var tmap_layer: TileMapLayer = get_parent()

var mstate_loaded: bool = false

var mdest_portal_id: StringName

var marriving: Array[Being] 

@rpc("call_local")
func load_state(dict: Dictionary, gridpos: Vector2i) -> void:
	mstate_loaded = true
	self.mid = dict[Keys.ID]
	GameData.portals[mid] = self
	GameData.portals_gridpos[mid] = gridpos
	pass

#@rpc("call_local")func on_load() -> void: show()
#@rpc("call_local")func on_unload() -> void: hide()

	
func serialize() -> Dictionary: return {}

func _ready() -> void: 
	mtrigger_area.body_entered.connect(_on_body_entered)	
	mtrigger_area.body_exited.connect(_on_body_exited)
	
		
#TODO: que se lleve a los followers
func _on_body_entered(body: Node2D) -> void:
	if (GameData.portals_gridpos.has(mdest_portal_id)) and body is Being and not marriving.has(body):
		if body.mistate.mfaction is PlayerFaction:
			body.global_position = tmap_layer.map_to_local(GameData.portals_gridpos[mdest_portal_id])
			if GameData.portals.has(mdest_portal_id):
				var dest: Portal = GameData.portals[mdest_portal_id]
				dest._on_receive_tp(body) 
				dest.marriving.append(body)

func _on_body_exited(body: Node2D) -> void:
	if body is Being:
		marriving.erase(body)

#TODO: disallow construction below a portal
