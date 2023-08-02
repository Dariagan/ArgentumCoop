extends Resource
class_name SlotData

const MAX: int = 99

@export var contained_item: ItemData

@export_range(1, MAX) var quantity: int = 1: set = set_quantity

func set_quantity(value: int) -> void:
	if value <= MAX:
		quantity = value
	else:
		push_error("%s exceeds max stack size, setting quantity to max" % contained_item.name)
		quantity = MAX
