#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Shader.h"
#include "Model.h"
#include "Terrain.h"
#include <stb_image/stb_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

extern "C" {
	_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

void draw_model(Model model, Shader shader, glm::vec3 camera_position, glm::mat4 m_model, glm::mat4 m_view, glm::mat4 m_projection);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void process_input(GLFWwindow* window);

void mouse_position_callback(GLFWwindow* window, double x_position, double y_position);

void render_debug_menu();

void render_light_source(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 color);

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
#define LOGL_FULLSCREEN

#ifdef LOGL_FULLSCREEN
int window_width = 1920;
int window_height = 1080;
bool is_window_fullscreen = true;
#else
int window_width = 1280;
int window_height = 720;
bool is_window_fullscreen = false;
#endif

//glm::vec3 clear_color = glm::vec3(194.0f / 255.0f, 224.0f / 255.0f, 1.0f);
glm::vec3 clear_color = glm::vec3(0.0f, 0.0f, 0.0f);

// mouse input variables
float mouse_last_x = window_width / 2.0f;
float mouse_last_y = window_height / 2.0f;
float mouse_sensitivity = 0.05f;

// debug menu variables
bool show_debug_menu = false;

// point light variables
std::vector<glm::vec3> lightPositions = {
	glm::vec3(-0.42f, 1.5f, -3.10f),
	glm::vec3(-0.42f, 1.5f, 0.75f),
	glm::vec3(-0.42f, 1.5f, 4.4f),
};

/*std::vector<glm::vec3> lightColors = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
};*/

std::vector<glm::vec3> lightColors = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
};

float point_light_linear = 0.3f;
float point_light_quadratic = 0.4f;

// ambient light variables
glm::vec3 directional_light_direction = glm::vec3(0.5f, 50.0f, -1.0f);
//glm::vec3 directional_light_direction = glm::vec3(-2.0f, 4.0f, -1.0f);
glm::vec3 directional_light_ambient = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 directional_light_diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
glm::vec3 directional_light_specular = glm::vec3(1.0f, 1.0f, 1.0f);

// sky variables
float red_factor = 0.18;
float green_factor = 0.27;
float blue_factor = 0.47;

// render target variable
int show_render_target = 0;
bool show_shadow_map = false;

// pp variables
float hdr_exposure = 1.0f;
int is_bloom = 1;
int bloom_blur_amount = 5;

float bloom_intensity_threshold = 0.6f;
int shadows_enabled = 1;
int specular_enabled = 1;

// shadow map variables
float sm_frustum_size = 50.0f;
float sm_near_plane = 1.0f;
float sm_far_plane = 100.0f;

// debug variables
bool is_renderdoc = false;

int main() {
	if (is_renderdoc) {
		std::cout << "press any key to start" << std::endl;
		std::cin.get();
	}
	
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

	/*Shader g_pass_terrain_shaders{ "Data/Shaders/v_g_pass_terrain.glsl", "Data/Shaders/f_g_pass_terrain.glsl" };
	Shader g_pass_single_texture_terrain_shaders{ "Data/Shaders/v_g_pass_single_texture_terrain.glsl", "Data/Shaders/f_g_pass_single_texture_terrain.glsl" };*/
	Shader sky_shaders = { "Data/Shaders/Sky/v_sky.glsl", "Data/Shaders/Sky/f_sky.glsl" };
	Shader g_pass_shaders{ "Data/Shaders/v_g_pass.glsl", "Data/Shaders/f_g_pass.glsl" };
	Shader deferred_shaders{ "Data/Shaders/v_deferred_render.glsl", "Data/Shaders/f_deferred_render.glsl" };
	Shader light_source_shaders = { "Data/Shaders/v_light_source.glsl", "Data/Shaders/f_light_source.glsl" };

	Shader simple_depth_shaders = { "Data/Shaders/v_simple_depth.glsl", "Data/Shaders/f_simple_depth.glsl" };
	Shader debug_depth_quad_shaders = { "Data/Shaders/v_debug_depth_quad.glsl", "Data/Shaders/f_debug_depth_quad.glsl" };

	Shader billboard_shaders = { "Data/Shaders/v_billboard.glsl", "Data/Shaders/f_billboard.glsl" };

	Shader hdr_shaders = { "Data/Shaders/v_hdr.glsl", "Data/Shaders/f_hdr.glsl" };
	Shader bloom_shaders = { "Data/Shaders/v_bloom.glsl", "Data/Shaders/f_bloom.glsl" };
	Shader blur_shaders = { "Data/Shaders/v_blur.glsl", "Data/Shaders/f_blur.glsl" };

	// Set callback function for window / frame size change so the viewport gets resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	Model sponza_model("Data/Models/Sponza/sponza.obj");
	Model sun_model("Data/Models/Sun/sun.obj");
	Model sivir_model("Data/Models/Sivir/sivir.obj");
	Model janna_model("Data/Models/Janna/janna.obj");
	Model med_house_model("Data/Models/MedievalHouse/medieval_house.obj");
	/*Model evelynn_model("Data/Models/Evelynn/evelynn.obj");
	Model house_model("Data/Models/House/house.obj");*/

	Model skydome_model("Data/Models/Dome/dome2.obj");

	/*Terrain main_terrain(100,
		"Data/Textures/levels/heightmap2.png", 
		"Data/Textures/levels/heightmap2_splatmap.png", 
		"Data/Textures/terrain/sand.jpg", 
		"Data/Textures/terrain/grass.jpg", 
		"Data/Textures/terrain/rock.jpg");
	Model terrain_model = main_terrain.GetModel();*/

	/*Terrain grand_canyon_terrain(25,
		"Data/Textures/levels/gcanyon_heightmap.png",
		"Data/Textures/levels/gcanyon_texturemap.png");
	Model terrain_model = grand_canyon_terrain.GetModel();*/

	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedoSpec, gDepth;

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - depth buffer
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gDepth, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(5, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete!" << std::endl;
	}

	unsigned int directional_light_depth_fbo;
	glGenFramebuffers(1, &directional_light_depth_fbo);
	unsigned int directional_light_depth_map;
	glGenTextures(1, &directional_light_depth_map);
	glBindTexture(GL_TEXTURE_2D, directional_light_depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, directional_light_depth_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directional_light_depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	deferred_shaders.Use();
	deferred_shaders.SetInt("gPosition", 0);
	deferred_shaders.SetInt("gNormal", 1);
	deferred_shaders.SetInt("gAlbedoSpec", 2);
	deferred_shaders.SetInt("gDepth", 3);
	deferred_shaders.SetInt("shadowMap", 4);
	
	unsigned int quad_vao = 0;
	unsigned int quad_vbo;

	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
		);
	}
	// create and attach depth buffer (renderbuffer)
	unsigned int rboHdrDepth;
	glGenRenderbuffers(1, &rboHdrDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboHdrDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboHdrDepth);
	unsigned int attachments_hdr[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments_hdr);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer not complete!" << std::endl;
		}
	}

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

		glm::mat4 view;
		view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 500.0f);

		glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightProjection = glm::ortho(-sm_frustum_size, sm_frustum_size, -sm_frustum_size, sm_frustum_size, sm_near_plane, sm_far_plane);
		glm::mat4 lightView = glm::lookAt(directional_light_direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		simple_depth_shaders.Use();
		simple_depth_shaders.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

		glBindFramebuffer(GL_FRAMEBUFFER, directional_light_depth_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		// draw janna
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			simple_depth_shaders.SetMatrix4("model", model);
			janna_model.Draw(simple_depth_shaders);
		}

		// draw house
		if (false)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			simple_depth_shaders.SetMatrix4("model", model);
			med_house_model.Draw(simple_depth_shaders);
		}

		// draw sponza
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			simple_depth_shaders.SetMatrix4("model", model);
			sponza_model.Draw(simple_depth_shaders);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (show_shadow_map) {
			debug_depth_quad_shaders.Use();
			debug_depth_quad_shaders.SetFloat("near_plane", sm_near_plane);
			debug_depth_quad_shaders.SetFloat("far_plane", sm_far_plane);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, directional_light_depth_map);
			if (quad_vao == 0)
			{
				float quadVertices[] = {
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				};
				glGenVertexArrays(1, &quad_vao);
				glGenBuffers(1, &quad_vbo);
				glBindVertexArray(quad_vao);
				glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			}

			glBindVertexArray(quad_vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 model = glm::mat4(1.0f);
			g_pass_shaders.Use();
			g_pass_shaders.SetMatrix4("projection", projection);
			g_pass_shaders.SetMatrix4("view", view);

			// draw janna
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
				g_pass_shaders.SetMatrix4("model", model);
				janna_model.Draw(g_pass_shaders);
			}

			// draw house
			if (false)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
				g_pass_shaders.SetMatrix4("model", model);
				med_house_model.Draw(g_pass_shaders);
			}

			// draw sponza
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
				g_pass_shaders.SetMatrix4("model", model);
				sponza_model.Draw(g_pass_shaders);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			deferred_shaders.Use();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, gDepth);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, directional_light_depth_map);

			for (unsigned int i = 0; i < lightPositions.size(); i++) {
				deferred_shaders.SetVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
				deferred_shaders.SetVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
				const float constant = 1.0;

				deferred_shaders.SetFloat("lights[" + std::to_string(i) + "].Linear", point_light_linear);
				deferred_shaders.SetFloat("lights[" + std::to_string(i) + "].Quadratic", point_light_quadratic);
				const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
				float radius = (-point_light_linear + std::sqrt(point_light_linear * point_light_linear - 4 * point_light_quadratic *
					(constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * point_light_quadratic);
				deferred_shaders.SetFloat("lights[" + std::to_string(i) + "].Radius", radius);
			}

			deferred_shaders.SetVec3("directional_light.direction", directional_light_direction);
			deferred_shaders.SetVec3("directional_light.ambient", directional_light_ambient);
			deferred_shaders.SetVec3("directional_light.diffuse", directional_light_diffuse);
			deferred_shaders.SetVec3("directional_light.specular", directional_light_specular);

			deferred_shaders.SetVec3("viewPos", camera_position);
			deferred_shaders.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
			deferred_shaders.SetInt("show_render_target", show_render_target);
			deferred_shaders.SetFloat("bloom_threshold", bloom_intensity_threshold);
			deferred_shaders.SetInt("shadows_enabled", (int)shadows_enabled);
			deferred_shaders.SetInt("specular_enabled", (int)specular_enabled);

			if (quad_vao == 0)
			{
				float quadVertices[] = {
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				};
				glGenVertexArrays(1, &quad_vao);
				glGenBuffers(1, &quad_vbo);
				glBindVertexArray(quad_vao);
				glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			}

			glBindVertexArray(quad_vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFBO);
			glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

			// render point light sources
			/*for (unsigned int i = 0; i < lightPositions.size(); i++) {
				glm::mat4 model = glm::mat4(1.0);
				model = glm::translate(model, lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
				render_light_source(light_source_shaders, model, view, projection, lightColors[i]);
			}*/

			// render skydome
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::scale(model, glm::vec3(250.0f, 250.0f, 250.0f));
				sky_shaders.Use();
				sky_shaders.SetMatrix4("model", model);
				sky_shaders.SetMatrix4("view", view);
				sky_shaders.SetMatrix4("projection", projection);
				sky_shaders.SetFloat("red_factor", red_factor);
				sky_shaders.SetFloat("green_factor", green_factor);
				sky_shaders.SetFloat("blue_factor", blue_factor);
				skydome_model.Draw(sky_shaders);
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// render sun
			{
				billboard_shaders.Use();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, directional_light_direction);
				model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

				billboard_shaders.SetMatrix4("model", model);
				billboard_shaders.SetMatrix4("view", view);
				billboard_shaders.SetMatrix4("projection", projection);

				sun_model.Draw(billboard_shaders);
			}

			glDisable(GL_BLEND);

			// 2. blur bright fragments with two-pass Gaussian Blur 
			// --------------------------------------------------
			bool horizontal = true, first_iteration = true;
			blur_shaders.Use();
			for (unsigned int i = 0; i < bloom_blur_amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				blur_shaders.SetInt("horizontal", horizontal);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
				glBindVertexArray(quad_vao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
			// --------------------------------------------------------------------------------------------------------------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			bloom_shaders.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
			bloom_shaders.SetInt("scene", 0);
			bloom_shaders.SetInt("bloomBlur", 1);
			bloom_shaders.SetInt("bloom", is_bloom);
			bloom_shaders.SetFloat("exposure", hdr_exposure);
			glBindVertexArray(quad_vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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

void draw_model(Model model, Shader shader, glm::vec3 camera_position, glm::mat4 m_model, glm::mat4 m_view, glm::mat4 m_projection) {
	shader.Use();

	// Set material properties
	shader.SetVec3("camera_position", camera_position);
	shader.SetFloat("material.shininess", 32.0f);

	shader.SetMatrix4("model", m_model);
	shader.SetMatrix4("view", m_view);
	shader.SetMatrix4("projection", m_projection);
	model.Draw(shader);
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
		ImGui::Checkbox("Show Shadow Map", &show_shadow_map);

		ImGui::Separator();
		ImGui::DragFloat("SM Frustum Size", &sm_frustum_size);
		ImGui::DragFloat("SM Near Plane", &sm_near_plane);
		ImGui::DragFloat("SM Far Plane", &sm_far_plane);

		ImGui::Separator();
		ImGui::DragFloat("Exposure", &hdr_exposure, 0.1f, 0.5f, 10.0f);
		ImGui::DragInt("Bloom Enabled", &is_bloom, 1, 0, 1);
		ImGui::DragInt("Bloom Blur Amount", &bloom_blur_amount, 1, 1, 10);
		ImGui::DragFloat("Bloom Intens. Thresh.", &bloom_intensity_threshold, 0.05f, 0.0f, 1.0f);
		ImGui::DragInt("Shadows Enabled", &shadows_enabled, 1, 0, 1);
		ImGui::DragInt("Specular Lights Enabled", &specular_enabled, 1, 0, 1);

		ImGui::Separator();
		ImGui::ColorEdit3("Clear Color", (float*)&clear_color);

		ImGui::Separator();
		ImGui::DragInt("Show Render Target", &show_render_target, 1.0f, 0, 4);

		ImGui::Separator();
		ImGui::DragFloat("Camera Speed", &base_camera_speed, 0.5f, 1.0f, 50.0f);

		ImGui::Separator();
		ImGui::Text("Point Light");

		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			std::string pos_label = "Plight" + std::to_string(i) + " Pos.";
			std::string col_label = "Plight" + std::to_string(i) + " Col.";
			ImGui::DragFloat3(pos_label.c_str(), (float*)&lightPositions[i], 0.01f);
			ImGui::ColorEdit3(col_label.c_str(), (float*)&lightColors[i]);
			ImGui::Separator();
		}

		ImGui::DragFloat("Point Light Linear", &point_light_linear, 0.1f);
		ImGui::DragFloat("Point Light Quadratic", &point_light_quadratic, 0.1f);

		ImGui::Separator();
		ImGui::Text("Ambient Light");
		ImGui::DragFloat3("Direction", (float*)&directional_light_direction, 0.01f);
		ImGui::ColorEdit3("Ambient", (float*)&directional_light_ambient);
		ImGui::ColorEdit3("Diffuse", (float*)&directional_light_diffuse);
		ImGui::ColorEdit3("Specular", (float*)&directional_light_specular);

		ImGui::End();
	}

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

unsigned int light_source_vao = 0;
unsigned int light_source_vbo = 0;
void render_light_source(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 color) {
	// initialize (if necessary)
	if (light_source_vao == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, // bottom-left
			 1.0f,  1.0f, -1.0f, // top-right
			 1.0f, -1.0f, -1.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f, // top-right
			-1.0f, -1.0f, -1.0f, // bottom-left
			-1.0f,  1.0f, -1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f, // bottom-left
			 1.0f, -1.0f,  1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f, // top-right
			 1.0f,  1.0f,  1.0f, // top-right
			-1.0f,  1.0f,  1.0f, // top-left
			-1.0f, -1.0f,  1.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, // top-right
			-1.0f,  1.0f, -1.0f, // top-left
			-1.0f, -1.0f, -1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f, // top-left
			 1.0f, -1.0f, -1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f, // top-right         
			 1.0f, -1.0f, -1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f, // top-left
			 1.0f, -1.0f,  1.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f, // top-right
			 1.0f, -1.0f, -1.0f, // top-left
			 1.0f, -1.0f,  1.0f, // bottom-left
			 1.0f, -1.0f,  1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, // bottom-right
			-1.0f, -1.0f, -1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f, // top-left
			 1.0f,  1.0f , 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f, // top-right     
			 1.0f,  1.0f,  1.0f, // bottom-right
			-1.0f,  1.0f, -1.0f, // top-left
			-1.0f,  1.0f,  1.0f, // bottom-left        
		};
		glGenVertexArrays(1, &light_source_vao);
		glGenBuffers(1, &light_source_vbo);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, light_source_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(light_source_vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	shader.Use();
	shader.SetMatrix4("model", model);
	shader.SetMatrix4("view", view);
	shader.SetMatrix4("projection", projection);
	shader.SetVec3("light_color", color);

	glBindVertexArray(light_source_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
