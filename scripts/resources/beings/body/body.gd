
# IMPORTANTE: usar .duplicate(true) desp de arrastrar un .tres de este recurso a algun @export de una var

extends Resource
class_name HarmeableBody

@export var body_tree: BodyTree

@export var body_groups: Array[BodyGroup] = []

func rec(body_tree: BodyTree):
	
	for belonging_to_group_name in body_tree.body_groups:
		for group in body_groups:
			if belonging_to_group_name == group._name:
				group.found_body_parts.push_back(body_tree.body_part)
	
	for child in body_tree.children:
		rec(child)

