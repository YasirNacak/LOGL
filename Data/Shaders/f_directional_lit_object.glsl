#version 330 core

struct Material {
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 directional_light_influence(DirectionalLight light_source, vec3 normal, vec3 camera_direction);

in vec2 texture_coords;
in vec3 vertex_normal;
in vec3 vertex_world_position;

out vec4 out_color;

uniform Material material;
uniform DirectionalLight directional_light;
uniform vec3 camera_position;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 camera_direction = normalize(camera_position - vertex_world_position);
    vec3 result = directional_light_influence(directional_light, normal, camera_direction);
    out_color = vec4(result, 1.0);
}

vec3 directional_light_influence(DirectionalLight light_source, vec3 normal, vec3 camera_direction) {
    vec3 light_direction = normalize(-light_source.direction);

    float diffuse_factor = max(dot(normal, light_direction), 0.0);

    vec3 reflection_direction = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);

    vec3 ambient  = light_source.ambient * texture(material.texture_diffuse0, texture_coords).rgb;
    vec3 diffuse = light_source.diffuse * diffuse_factor * texture(material.texture_diffuse0, texture_coords).rgb;
    vec3 specular = light_source.specular * specular_factor * texture(material.texture_specular0, texture_coords).rgb;

    return ambient + diffuse + specular;
}
