extends Control

@onready var inventory: PanelContainer = $Inventory


func set_player_inventory_data(inventory_data: InventoryData) -> void:
	inventory.set_inventory_data(inventory_data)

