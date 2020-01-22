#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "stb_image.h"

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

	Shader basic_shaders{"Data/Shaders/v_basic.glsl", "Data/Shaders/f_basic.glsl"};

	// Texture loading 1
	unsigned int tex1_id;
	glGenTextures(1, &tex1_id);
	glBindTexture(GL_TEXTURE_2D, tex1_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int tex1_width, tex1_height, tex1_n_channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* tex1_data = stbi_load("Data/Textures/plate_grey.jpg", &tex1_width, &tex1_height, &tex1_n_channels, 0);
	stbi_set_flip_vertically_on_load(false);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1_width, tex1_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex1_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex1_data);

	// Texture loading 2
	unsigned int tex2_id;
	glGenTextures(1, &tex2_id);
	glBindTexture(GL_TEXTURE_2D, tex2_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int tex2_width, tex2_height, tex2_n_channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* tex2_data = stbi_load("Data/Textures/smile.jpg", &tex2_width, &tex2_height, &tex2_n_channels, 0);
	stbi_set_flip_vertically_on_load(false);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex2_width, tex2_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex2_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex2_data);

	// Set callback function for window / frame size change so the viewport gets resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Create vertex array object to store upcoming VBO and EBO values in
	unsigned int vao;
	glGenVertexArrays(1, &vao);

	// Create vertex buffer object
	unsigned int vbo;
	glGenBuffers(1, &vbo);

	// Create element buffer object
	unsigned int ebo;
	glGenBuffers(1, &ebo);

	// Create array of vertices and assign it to an array buffer
	// This array includes both vertex positions, color values and texture coordinates
	// vX, vY, vZ, cR, cG, cB, tU, tV ...
	float vertices[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Draw loop
	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1_id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex2_id);

		basic_shaders.Use();

		basic_shaders.SetInt("texture1", 0);
		basic_shaders.SetInt("texture2", 1);

		// Switch to necessary VAO
		glBindVertexArray(vao);
		// Draw currently loaded tris using the currently loaded shader program and the currently loaded VAO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Unbind
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