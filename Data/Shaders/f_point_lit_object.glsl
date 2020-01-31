#version 330 core

struct Material {
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	float shininess;
};

struct PointLight { 
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 point_light_influence(PointLight light_source, vec3 normal, vec3 vertex_world_position, vec3 camera_direction);

in vec2 texture_coords;
in vec3 vertex_normal;
in vec3 vertex_world_position;

out vec4 out_color;

uniform Material material;
uniform PointLight point_light;
uniform vec3 camera_position;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 camera_direction = normalize(camera_position - vertex_world_position);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    result += point_light_influence(point_light, normal, vertex_world_position, camera_direction);    
    
    out_color = vec4(result, 1.0);
}

vec3 point_light_influence(PointLight light_source, vec3 normal, vec3 vertex_world_position, vec3 camera_direction) {
    vec3 light_direciton = normalize(light_source.position - vertex_world_position);
    
    float diffuse_factor = max(dot(normal, light_direciton), 0.0);
    
    vec3 reflection_direction = reflect(-light_direciton, normal);
    float specular_factor = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);
    
    float distance = length(light_source.position - vertex_world_position);
    float attenuation = 1.0 / 
    (light_source.constant + light_source.linear * distance + light_source.quadratic * (distance * distance));    
    
    vec3 ambient = light_source.ambient  * vec3(texture(material.texture_diffuse0, texture_coords));
    vec3 diffuse = light_source.diffuse  * diffuse_factor * vec3(texture(material.texture_diffuse0, texture_coords));
    vec3 specular = light_source.specular * specular_factor * vec3(texture(material.texture_specular0, texture_coords));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
