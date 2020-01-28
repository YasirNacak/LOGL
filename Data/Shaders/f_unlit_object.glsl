#version 330 core

struct Material {
	sampler2D texture_diffuse0;
};

in vec2 texture_coords;

out vec4 out_color;

uniform Material material;

void main() {
	out_color = texture(material.texture_diffuse0, texture_coords);
}