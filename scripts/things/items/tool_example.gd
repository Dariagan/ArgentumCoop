extends Resource

# reaccionado por el padre, este otorga lo q pide a la instancia myself
signal request(myself: Resource, giveme: Array[StringName])


@export var _whatever_is_spawned: PackedScene

var _target: Node2D

var _needed_stuff: Array

func _init() -> void:
	request.emit(self, ["world", "player"])


func fulfill_request(requested: Array):
	_needed_stuff = requested
	_target = _needed_stuff[2]
 #llamado por el parent despues de llamar tool.fulfill_request(). o sea haría:
# tool.fulfill_request(parent_things); tool.do_my_thing()
func do_my_thing(): #llamarlo execute
	pass
	#esto puede spawnear una escena custom donde sea (en el tilemap para evitar desyncs), y usar su _process para poner nodocustom.position = target.position
