#include "Shader.h"

Shader::Shader(const char* vert_path, const char* frag_path) {
	std::string vert_code;
	std::ifstream vert_fstream(vert_path);

	if (vert_fstream.is_open()) {
		std::stringstream vert_sstream;
		vert_sstream << vert_fstream.rdbuf();
		vert_fstream.close();
		vert_code = vert_sstream.str();
	}
	else {
		std::cerr << "SHADER FILE COULD NOT BE OPENED" << std::endl;
		return;
	}

	std::string frag_code;
	std::ifstream frag_fstream(frag_path);

	if (frag_fstream.is_open()) {
		std::stringstream frag_sstream;
		frag_sstream << frag_fstream.rdbuf();
		frag_fstream.close();
		frag_code = frag_sstream.str();
	}
	else {
		std::cerr << "SHADER FILE COULD NOT BE OPENED" << std::endl;
		return;
	}

	const char* vert_code_cstr = vert_code.c_str();
	const char* frag_code_cstr = frag_code.c_str();

	// Shader compilation control variables
	int is_shader_compiled;
	int are_shaders_linked;
	char shader_compile_log[512];
	char shader_link_log[512];

	// Load and compile vertex shader
	unsigned int vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_code_cstr, NULL);
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
	glShaderSource(fragment_shader, 1, &frag_code_cstr, NULL);
	glCompileShader(fragment_shader);

	// Check fragment shader compilation status
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (!is_shader_compiled) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, shader_compile_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << shader_compile_log << std::endl;
	}

	// Link shader together to create a shader program
	_program_id = glCreateProgram();
	glAttachShader(_program_id, vertex_shader);
	glAttachShader(_program_id, fragment_shader);
	glLinkProgram(_program_id);

	// Check shader link status
	glGetProgramiv(_program_id, GL_LINK_STATUS, &are_shaders_linked);
	if (!are_shaders_linked) {
		glGetProgramInfoLog(_program_id, 512, NULL, shader_link_log);
		std::cout << "ERROR:SHADER::LINK_FAILED\n" << shader_link_log << std::endl;
	}

	// delete already linked shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::Use() {
	glUseProgram(_program_id);
}

void Shader::SetBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(_program_id, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(_program_id, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(_program_id, name.c_str()), value);
}
