#version 330 core
in vec3 vertex_normal;
in vec3 vertex_world_position;

out vec4 out_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;
uniform float ambient_light_amount;
uniform float specular_light_amount;
uniform int specular_shininess;

void main() {
    vec3 light_direction = normalize(light_position - vertex_world_position);
    vec3 camera_direction = normalize(camera_position - vertex_world_position);
    vec3 reflection_direction = reflect(-light_direction, vertex_normal);

    float frag_specular_amount = pow(max(dot(camera_direction, reflection_direction), 0.0), pow(2, specular_shininess));
    vec3 specular_color = frag_specular_amount * specular_light_amount * light_color;

    float diffuse_amount = max(dot(vertex_normal, light_direction), 0.0);
    vec3 diffuse_color = diffuse_amount * light_color;

    out_color = vec4((specular_color + diffuse_color + ambient_light_amount) * object_color , 1.0f);
}