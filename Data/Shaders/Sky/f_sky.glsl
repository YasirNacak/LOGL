#version 330 core

in vec4 world_position;

out vec4 out_col;

const vec3 baseColor = vec3(0.18, 0.27, 0.47);

uniform float red_factor;
uniform float green_factor;
uniform float blue_factor;

void main() {
	float red = -0.0005 * (abs(world_position.y) - 500) + red_factor;
	float green = -0.0005 * (abs(world_position.y) - 500) + green_factor;
	float blue = -0.0005 * (abs(world_position.y) - 500) + blue_factor;
	
	out_col = vec4(red, green, blue, 1);
}
