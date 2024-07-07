extends Faction
class_name AiFaction
#hacer .duplicate() cuando se asigne en algun lugar

@export var culture: Culture

#este power es como la faction strength en el mod de lotr de warband, se reduce cuando pierde battles y aumenta si gana
@export var starting_faction_strength: int = 1000000
@export var raid_commonality_weight: int = 100
