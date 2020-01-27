#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outer_cutoff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

vec3 directional_light_influence(DirectionalLight light_source, vec3 normal, vec3 camera_direction);

vec3 point_light_influence(PointLight light_source, vec3 normal, vec3 vertex_world_position, vec3 camera_direction);

vec3 spot_light_influence(SpotLight light_source, vec3 normal, vec3 vertex_world_position, vec3 camera_direction);

#define N_POINT_LIGHTS 3

uniform Material material;
uniform DirectionalLight directional_light;
uniform PointLight point_lights[N_POINT_LIGHTS];
uniform SpotLight spot_light;
uniform vec3 camera_position;

in vec2 texture_coords;
in vec3 vertex_normal;
in vec3 vertex_world_position;

out vec4 out_color;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 camera_direction = normalize(camera_position - vertex_world_position);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    result += directional_light_influence(directional_light, normal, camera_direction);

    for(int i = 0; i < N_POINT_LIGHTS; i++) {
        result += point_light_influence(point_lights[i], normal, vertex_world_position, camera_direction);    
    }
    
    result += spot_light_influence(spot_light, normal, vertex_world_position, camera_direction);    
    
    out_color = vec4(result, 1.0);
}

vec3 directional_light_influence(DirectionalLight light_source, vec3 normal, vec3 camera_direction) {
    vec3 light_direction = normalize(-light_source.direction);

    float diffuse_factor = max(dot(normal, light_direction), 0.0);

    vec3 reflection_direction = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);

    vec3 ambient  = light_source.ambient * texture(material.diffuse, texture_coords).rgb;
    vec3 diffuse = light_source.diffuse * diffuse_factor * texture(material.diffuse, texture_coords).rgb;
    vec3 specular = light_source.specular * specular_factor * texture(material.diffuse, texture_coords).rgb;

    return ambient + diffuse + specular;
}

vec3 point_light_influence(PointLight light_source, vec3 normal, vec3 vertex_world_position, vec3 camera_direction) {
    vec3 light_direciton = normalize(light_source.position - vertex_world_position);
    
    float diffuse_factor = max(dot(normal, light_direciton), 0.0);
    
    vec3 reflection_direction = reflect(-light_direciton, normal);
    float specular_factor = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);
    
    float distance = length(light_source.position - vertex_world_position);
    float attenuation = 1.0 / 
    (light_source.constant + light_source.linear * distance + light_source.quadratic * (distance * distance));    
    
    vec3 ambient = light_source.ambient  * vec3(texture(material.diffuse, texture_coords));
    vec3 diffuse = light_source.diffuse  * diffuse_factor * vec3(texture(material.diffuse, texture_coords));
    vec3 specular = light_source.specular * specular_factor * vec3(texture(material.specular, texture_coords));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 spot_light_influence(SpotLight light_source, vec3 normal, vec3 vertex_world_position, vec3 camera_direction) {
    vec3 light_direction = normalize(light_source.position - vertex_world_position);

    float diffuse_factor = max(dot(normal, light_direction), 0.0);

    vec3 reflection_direction = reflect(-light_direction, normal);
    float specular_factor = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);

    float distance = length(light_source.position - vertex_world_position);
    float attenuation = 1.0 / (light_source.constant + light_source.linear * distance + light_source.quadratic * (distance * distance));    

    float theta = dot(light_direction, normalize(-light_source.direction)); 
    float epsilon = light_source.cutoff - light_source.outer_cutoff;
    float intensity = clamp((theta - light_source.outer_cutoff) / epsilon, 0.0, 1.0);
    
    vec3 ambient = light_source.ambient * vec3(texture(material.diffuse, texture_coords));
    vec3 diffuse = light_source.diffuse * diffuse_factor * vec3(texture(material.diffuse, texture_coords));
    vec3 specular = light_source.specular * specular_factor * vec3(texture(material.specular, texture_coords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
