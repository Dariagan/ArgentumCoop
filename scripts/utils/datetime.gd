extends Resource
class_name DateTime

@export_range(0, 59) var seconds: int = 0
@export_range(0, 59) var minutes: int = 0
@export_range(0, 23) var hours: int = 0
@export_range(0, 59) var days: int = 0

var mdelta_time: float

func increase(delta: float) -> void:
	mdelta_time += delta
	if mdelta_time < 1: return
	
	var delta_secs: int = mdelta_time
	mdelta_time -= delta_secs
	
	seconds += delta_secs
	minutes += seconds/60
	hours += minutes/60
	days += hours/24
	
	seconds %= 60
	minutes %= 60
	hours %= 24
	
	pass
