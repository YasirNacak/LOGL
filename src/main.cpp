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

unsigned int load_texture(const char* path);

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
glm::vec3 clear_color = glm::vec3(0.0f, 0.0f, 0.0f);

//int window_width = 1920;
//int window_height = 1080;
//bool is_window_fullscreen = true;

// mouse input variables
float mouse_last_x = window_width / 2.0f;
float mouse_last_y = window_height / 2.0f;
float mouse_sensitivity = 0.05f;

// debug menu variables
bool show_debug_menu = false;

// object data

// cube positions
#define CUBE_COUNT 3
glm::vec3 cube_positions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
};

// point light positions
#define POINT_LIGHT_COUNT 3
glm::vec3 point_light_positions[] = {
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(-1.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 1.0f, 1.0f),
};

// point light colors
glm::vec3 point_light_colors[] = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
};

// ambient light variables
glm::vec3 directional_light_direction = glm::vec3(-0.2f, -1.0f, -0.3f);
glm::vec3 directional_light_ambient = glm::vec3(0.05f, 0.05f, 0.05f);
glm::vec3 directional_light_diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
glm::vec3 directional_light_specular = glm::vec3(0.5f, 0.5f, 0.5f);

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
	// This array includes vertex positions, normals and texture coordinates
	float cube_vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// cube
	unsigned int cube_vao;
	glGenVertexArrays(1, &cube_vao);

	unsigned int cube_vbo;
	glGenBuffers(1, &cube_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(cube_vao);

	// vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load textures
	unsigned int cube_diffuse_texture = load_texture("Data/Textures/container_diffuse.jpg");
	unsigned int cube_specular_texture = load_texture("Data/Textures/container_specular.jpg");

	// light
	unsigned int light_vao;
	glGenVertexArrays(1, &light_vao);

	unsigned int light_vbo;
	glGenBuffers(1, &light_vbo);

	glBindVertexArray(light_vao);
	glBindBuffer(GL_ARRAY_BUFFER, light_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
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

		glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
		
		// Draw Light Source

		// directional light
		lit_object_shaders.Use();
		lit_object_shaders.SetVec3("directional_light.direction", directional_light_direction);
		lit_object_shaders.SetVec3("directional_light.ambient", directional_light_ambient);
		lit_object_shaders.SetVec3("directional_light.diffuse", directional_light_diffuse);
		lit_object_shaders.SetVec3("directional_light.specular", directional_light_specular);

		// spot light
		lit_object_shaders.SetVec3("spot_light.position", camera_position);
		lit_object_shaders.SetVec3("spot_light.direction", camera_front);
		lit_object_shaders.SetVec3("spot_light.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		lit_object_shaders.SetVec3("spot_light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		lit_object_shaders.SetVec3("spot_light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lit_object_shaders.SetFloat("spot_light.constant", 1.0f);
		lit_object_shaders.SetFloat("spot_light.linear", 0.09);
		lit_object_shaders.SetFloat("spot_light.quadratic", 0.032);
		lit_object_shaders.SetFloat("spot_light.cutoff", glm::cos(glm::radians(12.5f)));
		lit_object_shaders.SetFloat("spot_light.outer_cutoff", glm::cos(glm::radians(15.0f)));

		// Draw Point Lights
		for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
			lit_object_shaders.Use();
			char buffer[128];

			sprintf_s(buffer, "point_lights[%i].position", i);
			lit_object_shaders.SetVec3(buffer, point_light_positions[i]);

			sprintf_s(buffer, "point_lights[%i].constant", i);
			lit_object_shaders.SetFloat(buffer, 1.0f);

			sprintf_s(buffer, "point_lights[%i].linear", i);
			lit_object_shaders.SetFloat(buffer, 0.09f);

			sprintf_s(buffer, "point_lights[%i].quadratic", i);
			lit_object_shaders.SetFloat(buffer, 0.032f);

			sprintf_s(buffer, "point_lights[%i].diffuse", i);
			lit_object_shaders.SetVec3(buffer, point_light_colors[i]);

			sprintf_s(buffer, "point_lights[%i].ambient", i);
			lit_object_shaders.SetVec3(buffer, glm::vec3(0.1f));

			sprintf_s(buffer, "point_lights[%i].specular", i);
			lit_object_shaders.SetVec3(buffer, glm::vec3(1.0f));

			light_source_shaders.Use();
			light_source_shaders.SetVec3("light_color", point_light_colors[i]);
			
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, point_light_positions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			light_source_shaders.SetMatrix4("model", model);
			light_source_shaders.SetMatrix4("view", view);
			light_source_shaders.SetMatrix4("projection", projection);

			glBindVertexArray(light_vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		// Draw Cubes
		for(int i = 0 ; i < CUBE_COUNT; i++)
		{
			lit_object_shaders.Use();

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cube_positions[i]);

			lit_object_shaders.SetMatrix4("model", model);
			lit_object_shaders.SetMatrix4("view", view);
			lit_object_shaders.SetMatrix4("projection", projection);

			// Set material properties
			lit_object_shaders.SetVec3("camera_position", camera_position);
			lit_object_shaders.SetVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
			lit_object_shaders.SetFloat("material.shininess", 32.0f);

			// Set texture maps
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cube_diffuse_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cube_specular_texture);
			lit_object_shaders.SetInt("material.diffuse", 0);
			lit_object_shaders.SetInt("material.specular", 1);

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

unsigned int load_texture(const char* path) {
	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, n_components;
	unsigned char* data = stbi_load(path, &width, &height, &n_components, 0);
	if (data) {
		GLenum format;
		if (n_components == 1) {
			format = GL_RED;
		}
		else if (n_components == 3) {
			format = GL_RGB;
		}
		else if (n_components == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return texture_id;
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
		show_debug_menu = true;
		can_control_camera = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		show_debug_menu = false;
		can_control_camera = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

	ImGui::SetNextWindowSize(ImVec2(window_width / 5.0f, window_height));
	ImGui::SetNextWindowBgAlpha(0.25f);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	if (ImGui::Begin("Render Variables", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove)) {
		ImGui::Checkbox("Wireframe", &is_wireframe);

		ImGui::Separator();

		ImGui::ColorEdit3("Clear Color", (float*)&clear_color);

		ImGui::Separator();

		ImGui::Text("Cubes");
		for (int i = 0; i < CUBE_COUNT; i++) {
			ImGui::PushID(i);
			ImGui::DragFloat3("Cube Pos.", (float*)&(cube_positions[i]), 0.01f);
			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Text("Ambient Light");
		ImGui::DragFloat3("Direction", (float*)&directional_light_direction, 0.01f);
		ImGui::ColorEdit3("Ambient", (float*)&directional_light_ambient);
		ImGui::ColorEdit3("Diffuse", (float*)&directional_light_diffuse);
		ImGui::ColorEdit3("Specular", (float*)&directional_light_specular);

		ImGui::Separator();
		ImGui::Text("Spot Lights");
		for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
			ImGui::PushID(CUBE_COUNT + i);
			ImGui::DragFloat3("Point Light Pos.", (float*)&(point_light_positions[i]), 0.01f);
			ImGui::ColorEdit3("Point Light Col.", (float*)&(point_light_colors[i]));
			ImGui::PopID();
		}

		ImGui::End();
	}

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}