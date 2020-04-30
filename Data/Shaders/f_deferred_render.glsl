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
    vec3 specular;
};

in vec2 texture_coords;

layout (location = 0) out vec4 out_col;
layout (location = 1) out vec4 bright_col;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;
uniform sampler2D shadowMap;

const int NR_LIGHTS = 4;

uniform Light lights[NR_LIGHTS];
uniform DirectionalLight directional_light;
uniform vec3 viewPos;
uniform mat4 lightSpaceMatrix;

uniform int shadows_enabled;
uniform int specular_enabled;
uniform float bloom_threshold;

uniform int show_render_target;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 Normal, vec3 FragPos)
{
    if(shadows_enabled == 0) {
        return 1.0;
    }

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(directional_light.direction - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
        
    return shadow;
}  

vec3 directional_light_influence(DirectionalLight light_source, vec3 normal, vec3 camera_direction, vec3 diffuse_value, float spec_value, float shadow) {
    vec3 light_direction = normalize(light_source.direction);

    float diffuse_factor = max(dot(normal, light_direction), 0.0);

    vec3 reflection_direction = reflect(-light_direction, normal);
    float specular_factor = max(dot(camera_direction, reflection_direction), 0.0);

    vec3 ambient  = light_source.ambient * diffuse_value;
    vec3 diffuse = light_source.diffuse * diffuse_factor * diffuse_value;
    vec3 specular = light_source.specular * specular_factor * spec_value;

    if(specular_enabled == 0) {
        vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * diffuse_value;
        return lighting;
    }
    else {
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuse_value;
        return lighting;
    }
}

void main() {
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, texture_coords).rgb;
    vec3 Normal = texture(gNormal, texture_coords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, texture_coords).rgb;
    float Specular = texture(gAlbedoSpec, texture_coords).a;
    float Depth = texture(gDepth, texture_coords).x;

    //vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec4 pos_to_dir_light = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(pos_to_dir_light, Normal, FragPos);
    vec3 dir_light_inf = directional_light_influence(directional_light, Normal, viewDir, Diffuse, Specular, shadow);
    vec3 lighting = dir_light_inf;

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

    if(show_render_target == 0) {
        out_col = vec4(lighting, 1.0);
        if(length(out_col.rgb) > bloom_threshold) {
            bright_col = out_col;
        }
        else {
            bright_col = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }
    else if(show_render_target == 1) {
        out_col = vec4(Normal, 1.0);
    }
    else if(show_render_target == 2) {
        out_col = vec4(Diffuse, 1.0);
    }
    else if(show_render_target == 3) {
        out_col = vec4(FragPos, 1.0);
    }
    else if(show_render_target == 4) {
        out_col = vec4(Depth, Depth, Depth, 1.0);
    }
}