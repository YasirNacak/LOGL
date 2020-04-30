#version 330 core

in vec2 texture_coords;

out vec4 out_color;

uniform sampler2D texture_diffuse0;

void main() {
	vec4 col = texture(texture_diffuse0, texture_coords);
	out_color = col;
}
