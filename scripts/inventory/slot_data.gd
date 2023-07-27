extends Resource
class_name SlotData

const MAX: int = 99

@export var item: Item

@export_range(1, MAX) var quantity: int = 1: set = set_quantity

func set_quantity(value: int) -> void:
	if value <= item.max_stack_size:
		quantity = value
	else:
		push_error("%s exceeds max stack size, setting quantity to max" % item.name)
		quantity = item.max_stack_size
