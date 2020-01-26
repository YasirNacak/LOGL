#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	Shader(const char* vert_path, const char* frag_path);
	
	void Use();

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetMatrix4(const std::string& name, glm::mat4 value) const;
	void SetVec3(const std::string& name, glm::vec3 value) const;

	unsigned const int GetId() const;

private:
	unsigned int _program_id;
};
