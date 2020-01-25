#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void process_input(GLFWwindow* window);

void mouse_position_callback(GLFWwindow* window, double x_position, double y_position);

// Global variables (that will be moved to separate class)

// camera variables
glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float base_camera_speed = 2.5f;
float camera_pitch = 0.0f;
float camera_yaw = -90.0f;

// frame - time variables
float delta_time = 0.0f;
float last_frame_time = 0.0f;

// window variables
//int window_width = 1280;
//int window_height = 720;
//bool is_window_fullscreen = false;

int window_width = 1920;
int window_height = 1080;
bool is_window_fullscreen = true;

// mouse input variables
float mouse_last_x = window_width / 2.0f;
float mouse_last_y = window_height / 2.0f;
float mouse_sensitivity = 0.05f;

int main() {
	// Initialize GLFW / OpenGL variables
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWmonitor* window_monitor = is_window_fullscreen ? glfwGetPrimaryMonitor() : NULL;
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "LOGL", window_monitor, NULL);
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

	glViewport(0, 0, window_width, window_height);

	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_position_callback);

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

	// Create array of vertices and assign it to an array buffer
	// This array includes both vertex positions and texture coordinates
	// vX, vY, vZ, tU, tV ...
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Define 10 cubes to render
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// Draw loop
	while (!glfwWindowShouldClose(window)) {
		float current_frame_time = glfwGetTime();
		delta_time = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;
		process_input(window);

		glClearColor(0.0f, 46.0f / 255.0f, 102.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1_id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex2_id);

		basic_shaders.Use();

		basic_shaders.SetInt("texture1", 0);
		basic_shaders.SetInt("texture2", 1);

		// Set camera variables (work that is being done in LookAt)
		// glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
		// glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
		// glm::vec3 camera_direction = glm::normalize(camera_position - camera_target);
		// glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
		// glm::vec3 camera_right = glm::normalize(glm::cross(world_up, camera_direction));
		// glm::vec3 camera_up = glm::cross(camera_direction, camera_right);

		// Set view matrix based on the camera using LookAt
		glm::mat4 view;
		view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);

		// set matrix values for view and projection matrices
		int view_matrix_loc = glGetUniformLocation(basic_shaders.GetId(), "view");
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view));

		int projection_matrix_loc = glGetUniformLocation(basic_shaders.GetId(), "projection");
		glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));

		// Switch to necessary VAO
		glBindVertexArray(vao);
		
		// Render 10 cubes, setting each of their model matrix before rendering
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			int model_matrix_loc = glGetUniformLocation(basic_shaders.GetId(), "model");
			glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
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
	
	// Application exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	
	// Wireframe mode control
	if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if(glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Camera movement
	float current_camera_speed = base_camera_speed * delta_time;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_position += current_camera_speed * camera_front;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_position -= current_camera_speed * camera_front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * current_camera_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * current_camera_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera_position += current_camera_speed * camera_up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera_position += current_camera_speed * -camera_up;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		camera_pitch = 0.0f;
		camera_yaw = -90.0f;
		camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
		camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
		camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	
}

void mouse_position_callback(GLFWwindow* window, double x_position, double y_position) {
	float x_offset = x_position - mouse_last_x;
	float y_offset = mouse_last_y - y_position; // reversed since y-coordinates range from bottom to top
	mouse_last_x = x_position;
	mouse_last_y = y_position;

	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;

	camera_yaw += x_offset;
	camera_pitch += y_offset;

	if (camera_pitch > 89.0f) {
		camera_pitch = 89.0f;
	}
	if (camera_pitch < -89.0f) {
		camera_pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
	direction.y = sin(glm::radians(camera_pitch));
	direction.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
	camera_front = glm::normalize(direction);
}