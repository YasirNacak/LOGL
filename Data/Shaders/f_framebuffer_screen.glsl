#version 330 core
out vec4 out_color;

in vec2 texture_coords;

uniform sampler2D color_texture;
uniform sampler2D depth_texture;
uniform vec3 sky_color;
uniform float fog_intensity;

void main() {
    vec3 col = texture(color_texture, texture_coords).rgb;

	float depth = texture(depth_texture, texture_coords).x;
    out_color = vec4(depth, depth, depth, 1.0);
    out_color = vec4(mix(col, sky_color, clamp(depth * fog_intensity, 0.0, 1.0)), 1.0);
}
