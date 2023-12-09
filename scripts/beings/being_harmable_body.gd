
# IMPORTANTE: usar .duplicate(true) desp de arrastrar un .tres de este recurso a algun @export de una var
extends Node
class_name HarmableBody

@export var _body_tree: BodyTree

@export var body_groups: Array[BodyGroup] = []

func _init(data: Dictionary = {}):
	# TODO
	pass

#HACER QUE CADA BODY PART SEA UN NODE?

func rec(body_tree: BodyTree):
	
	for belonging_to_group_name in _body_tree.body_groups:
		for group in body_groups:
			if belonging_to_group_name == group._name:
				group.found_body_parts.push_back(_body_tree.body_part)
	
	for child in _body_tree.children:
		rec(child)

# TODO
func serialize() -> Dictionary:
	return {}
