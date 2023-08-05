extends Resource
class_name SlotData

@export var contained_item: ItemData

@export var quantity: int = 1: set = set_quantity

func set_quantity(value: int) -> void:
	if value == 0:
		contained_item = null
	elif value < 0:
		push_error("can't assign negative quantity to slot")
