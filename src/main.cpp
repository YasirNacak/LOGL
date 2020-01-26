#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#include <stb_image/stb_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void process_input(GLFWwindow* window);

void mouse_position_callback(GLFWwindow* window, double x_position, double y_position);

void render_debug_menu();

// Global variables (that will be moved to separate class)

// general render variables
bool is_wireframe = false;

// camera variables
bool can_control_camera = true;
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
int window_width = 1280;
int window_height = 720;
bool is_window_fullscreen = false;

//int window_width = 1920;
//int window_height = 1080;
//bool is_window_fullscreen = true;

// mouse input variables
float mouse_last_x = window_width / 2.0f;
float mouse_last_y = window_height / 2.0f;
float mouse_sensitivity = 0.05f;

// debug menu variables
bool show_debug_menu = false;

int main() {
	// Initialize GLFW / OpenGL variables
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSwapInterval(1); // Enable vsync

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

	// Initialize Imgui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glViewport(0, 0, window_width, window_height);

	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_position_callback);
	glfwSetCursorPos(window, mouse_last_x, mouse_last_y);

	Shader lit_object_shaders{ "Data/Shaders/v_lit_object.glsl", "Data/Shaders/f_lit_object.glsl" };
	Shader light_source_shaders{ "Data/Shaders/v_light_source.glsl", "Data/Shaders/f_light_source.glsl" };

	// Set callback function for window / frame size change so the viewport gets resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Create array of vertices and assign it to an array buffer
	// This array includes vertex positions
	float cube_vertices[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5,
		-0.5f,  0.5f, -0.5f
	};

	// CUBE
	unsigned int cube_vao;
	glGenVertexArrays(1, &cube_vao);

	unsigned int cube_vbo;
	glGenBuffers(1, &cube_vbo);

	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// LIGHT
	unsigned int light_vao;
	glGenVertexArrays(1, &light_vao);

	unsigned int light_vbo;
	glGenBuffers(1, &light_vbo);

	glBindVertexArray(light_vao);
	glBindBuffer(GL_ARRAY_BUFFER, light_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Draw loop
	while (!glfwWindowShouldClose(window)) {
		float current_frame_time = glfwGetTime();
		delta_time = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;
		process_input(window);

		if (is_wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
		
		// Draw Light Source
		glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
		{
			light_source_shaders.Use();

			glm::vec3 light_position = glm::vec3(1.2f, 1.0f, 2.0f);
			glm::vec3 light_scale = glm::vec3(0.2f);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light_position);
			model = glm::scale(model, light_scale);

			light_source_shaders.SetMatrix4("model", model);
			light_source_shaders.SetMatrix4("view", view);
			light_source_shaders.SetMatrix4("projection", projection);
			light_source_shaders.SetVec3("light_color", light_color);

			glBindVertexArray(light_vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		// Draw Cube
		{
			lit_object_shaders.Use();

			glm::vec3 cube_color = glm::vec3(1.0f, 0.5f, 0.31f);
			glm::vec3 cube_position = glm::vec3(0.0f, 0.0f, 0.0f);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cube_position);

			lit_object_shaders.SetMatrix4("model", model);
			lit_object_shaders.SetMatrix4("view", view);
			lit_object_shaders.SetMatrix4("projection", projection);
			lit_object_shaders.SetVec3("object_color", cube_color);
			lit_object_shaders.SetVec3("light_color", light_color);

			glBindVertexArray(cube_vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
		
		if (show_debug_menu) {
			render_debug_menu();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
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
	
	// Camera movement
	if (can_control_camera) {
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
	
	// Imgui enable - disable switch
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		can_control_camera = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		can_control_camera = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// GUI Mode - Camera Mode switch
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		show_debug_menu = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) {
		show_debug_menu = false;
	}
}

void mouse_position_callback(GLFWwindow* window, double x_position, double y_position) {
	if (!can_control_camera) return;

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

	camera_yaw = glm::mod(camera_yaw + x_offset, 360.0f);

	glm::vec3 direction;
	direction.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
	direction.y = sin(glm::radians(camera_pitch));
	direction.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
	camera_front = glm::normalize(direction);
}

void render_debug_menu() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize(ImVec2(window_width / 6, window_height));
	ImGui::SetNextWindowBgAlpha(0.25f);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	if (ImGui::Begin("Render Variables", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove)) {
		ImGui::Checkbox("Wireframe", &is_wireframe);
		ImGui::End();
	}

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}