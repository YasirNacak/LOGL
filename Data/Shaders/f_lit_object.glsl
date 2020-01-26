#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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

in vec3 vertex_normal;
in vec3 vertex_world_position;

out vec4 out_color;

void main() {
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 light_direction = normalize(light.position - vertex_world_position);
    float diff = max(dot(vertex_normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular
    vec3 camera_direction = normalize(camera_position - vertex_world_position);
    vec3 reflection_direction = reflect(-light_direction, vertex_normal);
    float spec = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
        
    vec3 result = ambient + diffuse + specular;
    out_color = vec4(result, 1.0);
}