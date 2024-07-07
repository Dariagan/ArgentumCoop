extends Resource
class_name Culture


# tendencia a friendly towards cultures: blablabla
# tendencia a aversion towards cultures: blablabla

@export var playable: bool = true

# lista nombres posibles generables para instancias de facciones IA con esta culture
@export var possible_fac_names: Array[String] = ["placeholder"]

@export var native_being_kinds: Array[BeingGenTemplate]

# TODO buildings
# TODO techs
# TODO bonuses por pop % en tu faction
# ...y races pertenecientes al bonus x, y, z
