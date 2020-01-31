#version 330 core
out vec4 out_color;

float near = 0.1; 
float far = 100.0;

float linearize_depth(float depth)  {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {             
    float depth = linearize_depth(gl_FragCoord.z) / far;
    out_color = vec4(vec3(depth), 1.0);
}
