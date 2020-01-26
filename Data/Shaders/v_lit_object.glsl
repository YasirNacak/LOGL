#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out vec3 vertex_normal;
out vec3 vertex_world_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vertex_normal = mat3(transpose(inverse(model))) * normalize(in_normal);
	
	vec4 pos_vec4 = vec4(in_pos, 1.0);

	// vec3 version for diffuse lighting, vec4 version for the mvp
	vec4 world_position = model * pos_vec4;
	vertex_world_position = vec3(world_position);

	gl_Position = projection * view * world_position;
}