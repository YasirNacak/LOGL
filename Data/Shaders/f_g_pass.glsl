#version 330 core
layout (location = 0) out vec3 out_f_position;
layout (location = 1) out vec3 out_f_normal;
layout (location = 2) out vec4 out_f_albedo_spec;
layout (location = 3) out vec4 out_f_depth;

in vec3 fragment_position;
in vec2 texture_coords;
in vec3 normal;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;

const float near = 0.1;
const float far = 500.0;

float linearize_depth(float depth)  {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
    out_f_position = fragment_position;
    out_f_normal = normalize(normal);
    out_f_albedo_spec.rgb = texture(texture_diffuse0, texture_coords).rgb;
    out_f_albedo_spec.a = texture(texture_specular0, texture_coords).r;
    
    float linear_depth = linearize_depth(gl_FragCoord.z) / far;

    out_f_depth = vec4(vec3(linear_depth), 1.0);
}
