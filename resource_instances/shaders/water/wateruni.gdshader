shader_type canvas_item;

uniform vec4 color: source_color;
uniform sampler2D noise: repeat_enable;
uniform sampler2D screen_content: hint_screen_texture;

uniform vec2 scroll = vec2(0.05, 0.05);
uniform vec2 scroll2 = vec2(-0.05, -0.05);

uniform float distortion_strength : hint_range(-1.0, 1.0, 0.1) = 0.2;
void fragment() {
	vec4 noise_col = texture(noise, UV + scroll * TIME);
	vec4 screen_col = texture(screen_content, SCREEN_UV + distortion_strength * noise_col.rr);
	
	COLOR = screen_col;
}

//void light() {
	// Called for every pixel for every light affecting the CanvasItem.
	// Uncomment to replace the default light processing function with this one.
//}
