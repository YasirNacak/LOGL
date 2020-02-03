#version 330 core
layout (location = 0) in vec3 v_in_pos;
layout (location = 1) in vec2 v_in_texture_coords;

out vec2 texture_coords;

void main() {
    texture_coords = v_in_texture_coords;
    gl_Position = vec4(v_in_pos, 1.0);
}