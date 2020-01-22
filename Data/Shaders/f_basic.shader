#version 330 core
out vec4 col;

in vec3 ourColor;

void main() {
	col = vec4(ourColor, 1.0);
}