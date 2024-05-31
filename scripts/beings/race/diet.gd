extends Resource
class_name Diet

@export var nutrition_values: Dictionary #key: food id, value: 0-3
@export var harm_values: Dictionary #key: food id, damage
@export var parasitation_probabilities: Dictionary #key: food id, prob

#CREO Q ES MUCHO MÁS SAFER UN ARRAY DE STRING, FLOAT 
