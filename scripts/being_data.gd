extends Resource
#data común para todos los seres vivos
class_name BeingData 

var health: int #hacer componente, o no no sé
var max_speed: int
var carried_weight: int
var faction: Faction
var race: BasicRace

# mejor ponerselo al script del sprite específico
var head_sprite_data: HeadSpriteData
var body_sprite_data: BodySpriteData


#tal vez es mejor q sea un dictionary
var head_wear: HeadWear
var body_wear: BodyWear


var inventory_data: InventoryData


