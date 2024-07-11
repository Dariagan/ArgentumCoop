shader_type canvas_item;

uniform sampler2D noise_texture: repeat_enable;
uniform sampler2D screen_texture: hint_screen_texture;

uniform vec2 speed = vec2(0.03, 0.07);
uniform float wave_strength = 2;
uniform vec4 color : source_color;

void fragment() {
	// Called for every pixel the material is visible on.
	vec2 uv = UV + speed * TIME;
	
	vec2 noise = texture(noise_texture, uv).rg;
	//TODO hacer q solo el noise se mueva su sampling
	COLOR = vec4(color.rgb, 1) + vec4(noise.x, noise.y, 1, 1);
	//esto agarra el color del fondo
	COLOR *= texture(screen_texture, SCREEN_UV + noise * (wave_strength/100.0));
}

//void light() {
	// Called for every pixel for every light affecting the CanvasItem.
	// Uncomment to replace the default light processing function with this one.
//}
