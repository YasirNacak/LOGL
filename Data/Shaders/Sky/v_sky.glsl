#version 330 core

layout (location = 0) in vec3 in_pos;

out vec4 world_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	world_position = (model * vec4(in_pos, 1));
	gl_Position = projection * view * world_position;
}
