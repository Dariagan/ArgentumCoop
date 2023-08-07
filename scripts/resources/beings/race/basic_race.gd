extends Resource
class_name BasicRace


@export var name: StringName = &" "
@export var icon: Texture2D

@export var head_sprites_datas: Array[SpriteData]
@export var body_sprites_datas: Array[SpriteData]
@export var other_sprites_datas: Array[SpriteData]

@export var demonym: String #gentilicio
@export var singular_denomination: String
@export var plural_denomination: String

@export_multiline var description: String 

@export var can_equip_tools: bool

@export_range(0, 1) var filth_generation: float = 0
@export_range(0, 3) var max_encumberance_multiplier: float = 1 #preferably leave at 1

@export var comfortable_temp_range: Vector2i = Vector2i(16, 23)

#@export var body: Body las partes del cuerpo, incluyendo si están incluidas en la generación de damage o no

@export_category("Eating")
@export_range(0, 3) var hunger_rate: float = 1
@export var diets: Array[Diet]



@export_category("Abilities")
@export var can_vent: bool = false
@export var can_climb: bool = false#hacer int de nivel 0-10?

@export var combat_multipliers: CombatMultipliers


#export array de tupla biome_id: string preferrance: float


@export_category("Population")
# value = 1 -> 100% of pawns are male 0 _> all pawns are female

@export_range(0, 1) var males_females_ratio: float = 0.5


