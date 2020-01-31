#version 330 core
out vec4 out_color;

in vec3 texture_coords;

uniform samplerCube skybox;

void main() {
    out_color = texture(skybox, texture_coords);
}