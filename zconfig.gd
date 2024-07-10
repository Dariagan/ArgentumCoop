extends Node

const default_ip_to_join: String = "localhost"
var ignore_joiners_readiness_on_start: bool = true
var insta_start: bool = 0
var debug: bool = 1
var debug_walk_mult:float = 3
var noclip: bool = 0
var enable_change_zoom: bool = 1
var enable_zoom_limit: bool = 0
var noclip_speed:float = 600
var zoom_in_max = 999*Vector2.ONE; const zoom_out_max = 1.51*Vector2.ONE
