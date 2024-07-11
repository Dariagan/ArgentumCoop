shader_type canvas_item;

uniform sampler2D screen_texture: hint_screen_texture;
uniform sampler2D noise_texture: filter_nearest, repeat_enable;

uniform float distortion = 0.001;

uniform vec2 speed = vec2(0.5, 0.2);

uniform float wave_strength = 0.7;

//para este shader la textura de la tile individual tiene que ocupar todo el sourceatlas

void vertex() {
	// Called for every vertex the material is visible on.
	float displacement = (sin(TIME) + 1.0)*5.0;
	VERTEX += vec2(displacement, displacement);
	
}

void fragment() {
	// Called for every pixel the material is visible on.
	vec2 uv = UV + speed * TIME;
	
	vec2 noise = 2.0 * texture(noise_texture, uv).rg - vec2(1.0);
	
	COLOR = texture(TEXTURE, UV + noise * wave_strength);
	//esto agarra el color del fondo
	COLOR *= texture(screen_texture, SCREEN_UV + noise * distortion);
}