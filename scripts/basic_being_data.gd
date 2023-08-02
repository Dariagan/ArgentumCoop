extends Resource
#data común para todos los seres vivos
class_name BasicBeingData 

var health: int #hacer componente, o no no sé
var max_speed: int
var carried_weight: int
var faction: Faction

var head_sprite_data: HeadSpriteData
var body_sprite_data: BodySpriteData

var head_wear: HeadWear
var body_wear: BodyWear


var inventory_data: InventoryData

var character_data#: CharacterData #puede ser null, desp fijarse con if character_data:

var animal_data#: AnimalData #puede ser null, desp fijarse con if character_data:
