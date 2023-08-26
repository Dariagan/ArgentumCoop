extends Resource
class_name ItemData

@export var id: String
@export var name: String
@export_multiline var description: String
@export var texture: Texture2D
@export_range(0,50) var weight: float
@export_range(0,50) var encumberance: float #should turn to 0 when worn
@export var stackable: bool


@export var melee: MeleeEquipment
@export var shootable: Shootable

@export var combat_multipliers: CombatMultipliers #these take effect on item worn OR equipped

@export var wear: Wear

@export var item_carrier: ItemCarrier

@export var tool: Resource #hacer un Array[Resource] más tarde (si resulta ser necesario)

#@export var durability: float = -1
#@export_exp_easing("inout") var decay 
