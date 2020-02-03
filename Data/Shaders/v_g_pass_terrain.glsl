#version 330 core
layout (location = 0) in vec3 v_in_pos;
layout (location = 1) in vec3 v_in_normal;
layout (location = 2) in vec2 v_in_texture_coords;

out vec3 fragment_position;
out vec2 tiled_texture_coords;
out vec2 default_texture_coords;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int tiling;

void main() {
    vec4 world_position = model * vec4(v_in_pos, 1.0);
    fragment_position = world_position.xyz; 
    tiled_texture_coords = v_in_texture_coords * tiling;
    default_texture_coords  = v_in_texture_coords;
    
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    normal = normal_matrix * v_in_normal;

    gl_Position = projection * view * world_position;
}
