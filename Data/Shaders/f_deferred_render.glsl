#version 330 core
struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

in vec2 texture_coords;

out vec4 out_col;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

const int NR_LIGHTS = 32;

uniform Light lights[NR_LIGHTS];
uniform DirectionalLight directional_light;
uniform vec3 viewPos;
uniform vec3 fog_color;
uniform float fog_intensity;

vec3 directional_light_influence(DirectionalLight light_source, vec3 normal, vec3 camera_direction, vec3 diffuse_map) {
    vec3 light_direction = normalize(-light_source.direction);

    float diffuse_factor = max(dot(normal, light_direction), 0.0);

    vec3 ambient  = light_source.ambient * diffuse_map;
    vec3 diffuse = light_source.diffuse * diffuse_factor * diffuse_map;

    return ambient + diffuse;
}

void main() {
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, texture_coords).rgb;
    vec3 Normal = texture(gNormal, texture_coords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, texture_coords).rgb;
    float Specular = texture(gAlbedoSpec, texture_coords).a;
    float Depth = texture(gDepth, texture_coords).x;

    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    lighting += directional_light_influence(directional_light, Normal, viewDir, Diffuse);
    
    /*for(int i = 0; i < NR_LIGHTS; ++i) {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius) {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights[i].Color * spec * Specular;
            // attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }*/
    out_col = vec4(mix(lighting, fog_color, clamp(Depth * fog_intensity, 0.0, 1.0)), 1.0);
}