extends DirectionalLight2D
class_name TimeSystem

const MINUTES_PER_DAY: int = 1440
const MINUTES_PER_HOUR: int = 60
const INGAME_TO_REAL_MINUTE_DURATION = (2 * PI) / MINUTES_PER_DAY
@onready var multiplayer_synchronizer: MultiplayerSynchronizer = $MultiplayerSynchronizer

signal time_tick(day: int, hour: int, minute: int)

@export var gradient_texture: GradientTexture1D

@export var INGAME_SPEED: float = 50.0:
	set(speed): INGAME_SPEED = speed; multiplayer_synchronizer.replication_interval = calc_repli_interval()

func calc_repli_interval() -> float: return 2/pow(INGAME_SPEED, 1.4)

@export var INITIAL_HOUR: int = 16:
	set(h): INITIAL_HOUR = h; time = INGAME_TO_REAL_MINUTE_DURATION * MINUTES_PER_HOUR * INITIAL_HOUR
		
var time: float = 0.0

func _ready() -> void:
	set_process(false)
	multiplayer_synchronizer.replication_interval = calc_repli_interval()
	time = INGAME_TO_REAL_MINUTE_DURATION * MINUTES_PER_HOUR * INITIAL_HOUR
	_process(0)
	
@rpc("call_local") func enable_process(): set_process(true)

#https://www.youtube.com/watch?v=HjwWe-V3nHs
#Engine.timescale

func _process(delta: float) -> void:
	time += delta * INGAME_TO_REAL_MINUTE_DURATION * INGAME_SPEED
	
	var value = (sin(time - PI / 2.0) + 1.0) / 2.0
	self.color = gradient_texture.gradient.sample(value)
	_recalculate_time()	
		
var past_minute: int= -1
func _recalculate_time() -> void:
	var total_minutes = int(time / INGAME_TO_REAL_MINUTE_DURATION)
	
	var day = int(total_minutes / MINUTES_PER_DAY)

	var current_day_minutes = total_minutes % MINUTES_PER_DAY

	var hour = int(current_day_minutes / MINUTES_PER_HOUR)
	var minute = int(current_day_minutes % MINUTES_PER_HOUR)
	
	if past_minute != minute:
		past_minute = minute
		time_tick.emit(day, hour, minute)
