extends StaticBody2D
#subject to changes

#assigned by user, not random
var mid: StringName
var mpos_for_arrivals: Vector2
@onready var mtrigger_area: Area2D = $Area2D

var mdest: Node2D
var disable_tp: bool = false

func _ready() -> void: 
	mpos_for_arrivals = self.global_position
	add_to_group(Keys.G_PORTALS_POI, true)
	mtrigger_area.body_entered.connect(_on_area_2d_body_entered)	
	
func set_dest_portal(dest_id: StringName):
	for poi: Node in get_tree().get_nodes_in_group(Keys.G_PORTALS_POI):
		if poi is Node2D and poi.mid and poi.mid == dest_id and poi.mpos_for_arrivals:
			mdest = poi
		
func _on_receive_tp(): disable_tp = true
	
func _on_area_2d_body_entered(body: Node2D) -> void:
	if mdest and body is Being and not disable_tp:
		if body.mistate.mfaction is PlayerFaction:
			if mdest.has_method(&"_on_receive_tp"):
				mdest._on_receive_tp()
			body.global_position = mdest.mpos_for_arrivals
