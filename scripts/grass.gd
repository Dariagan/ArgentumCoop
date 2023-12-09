extends Sprite2D
class_name DynamicTile #recorta las dimensiones de su noisemap según las tiles q tenga adyacente

func _init() -> void:
	pass
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

#va a haber q hacer las cosas de autotile programáticamente (cambiar la textura si hay agua al lado y acortar las dimensiones de la noiseimage)

#getparent() para acceder al tilemap, o tal vez señal
