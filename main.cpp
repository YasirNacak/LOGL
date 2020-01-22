#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

const char* simple_vertex_shader_src = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* simple_fragment_shader_src = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void process_input(GLFWwindow* window);

int main() {
	// Initialize GLFW / OpenGL variables
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "LOGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	// Shader compilation control variables
	int is_shader_compiled;
	int are_shaders_linked;
	char shader_compile_log[512];
	char shader_link_log[512];

	// Load and compile vertex shader
	unsigned int vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &simple_vertex_shader_src, NULL);
	glCompileShader(vertex_shader);
	
	// Check vertex shader compilation status
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (!is_shader_compiled) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, shader_compile_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << shader_compile_log << std::endl;
	}

	// Load and compile fragment shader
	unsigned int fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &simple_fragment_shader_src, NULL);
	glCompileShader(fragment_shader);

	// Check fragment shader compilation status
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (!is_shader_compiled) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, shader_compile_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << shader_compile_log << std::endl;
	}

	// Link shader together to create a shader program
	unsigned int shader_program;
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	// Check shader link status
	glGetProgramiv(shader_program, GL_LINK_STATUS, &are_shaders_linked);
	if (!are_shaders_linked) {
		glGetProgramInfoLog(shader_program, 512, NULL, shader_link_log);
		std::cout << "ERROR:SHADER::LINK_FAILED\n" << shader_link_log << std::endl;
	}

	// delete already linked shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Set callback function for window / frame size change so the viewport gets resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Create vertex array object to store upcoming VBO and EBO values in
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create array of vertices and assign it to an array buffer
	float vertices[] = {
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create EBO
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Draw loop
	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use previously linked shader program to render
		glUseProgram(shader_program);
		// Switch to necessary VAO
		glBindVertexArray(vao);
		// Draw currently loaded tris using the currently loaded shader program and the currently loaded VAO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
	// Input handling goes here...
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if(glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}