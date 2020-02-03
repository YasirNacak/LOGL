#pragma once

#include <string>
#include <iostream>

#include <stb_image/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Texture {
public:
	unsigned int Id;
	std::string Type;
	std::string Path;

	Texture();
	Texture(unsigned int id, std::string type, std::string path);

	static unsigned int Load(std::string path);
};