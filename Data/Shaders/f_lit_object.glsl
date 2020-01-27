#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 camera_position;

in vec2 texture_coords;
in vec3 vertex_normal;
in vec3 vertex_world_position;

out vec4 out_color;

void main() {
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, texture_coords).rgb;

    // diffuse
    vec3 light_direction = normalize(light.position - vertex_world_position);
    float diff = max(dot(vertex_normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texture_coords).rgb;

    // specular
    vec3 camera_direction = normalize(camera_position - vertex_world_position);
    vec3 reflection_direction = reflect(-light_direction, vertex_normal);
    float spec = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, texture_coords).rgb;
        
    vec3 result = ambient + diffuse + specular;
    out_color = vec4(result, 1.0);
}