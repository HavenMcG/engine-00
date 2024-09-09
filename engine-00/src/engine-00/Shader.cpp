#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertex_path, const char* fragment_path) {
	// 1. retrieve the vertex/fragment source code from file_path
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;
	// ensure ifstream objects can throw exceptions:
	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// open files
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		std::stringstream v_shader_stream, f_shader_stream;
		// read file's buffer contents into streams
		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();
		// close file handlers
		v_shader_file.close();
		f_shader_file.close();
		// convert stream into string
		vertex_code = v_shader_stream.str();
		fragment_code = f_shader_stream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* v_shader_code = vertex_code.c_str();
	const char* f_shader_code = fragment_code.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char info_log[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	// shader program
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	glLinkProgram(id_);
	// print linking errors if any
	glGetProgramiv(id_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id_, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// delete the shaders as they are linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader() {
	glDeleteProgram(id_);
}

unsigned int Shader::id() const {
	return id_;
}

void Shader::use() {
	glUseProgram(id_);
}

// utility uniform functions
void Shader::set_bool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(id(), name.c_str()), (int)value);
}

void Shader::set_int(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id(), name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id(), name.c_str()), value);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& value) {
	glUniform2fv(glGetUniformLocation(id(), name.c_str()), 1, &value[0]);
}

void Shader::set_vec2(const std::string& name, float x, float y) {
	glUniform2f(glGetUniformLocation(id(), name.c_str()), x, y);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) {
	glUniform3fv(glGetUniformLocation(id(), name.c_str()), 1, &value[0]);
}
void Shader::set_vec3(const std::string& name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(id(), name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) {
	glUniform4fv(glGetUniformLocation(id(), name.c_str()), 1, &value[0]);
}
void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(id(), name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string& name, const glm::mat2& mat) {
	glUniformMatrix2fv(glGetUniformLocation(id(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string& name, const glm::mat3& mat) {
	glUniformMatrix3fv(glGetUniformLocation(id(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3x2(const std::string& name, const glm::mat3x2& mat) {
	glUniformMatrix3x2fv(glGetUniformLocation(id(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(id(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::set_material(const std::string& name, const Material& material, const OglAssetStore& assets) {
	int total_textures = 0;
	for (int i = 0; i < material.diffuses.size(); ++i, ++total_textures) {
		set_int(name + ".diffuse_textures[" + std::to_string(i) + "]", total_textures);
		set_int(name + ".diffuse_blend_strengths[" + std::to_string(i) + "]", material.diffuses[i].blend_strength);
		set_int(name + ".diffuse_blend_ops[" + std::to_string(i) + "]", material.diffuses[i].blend_op);
	}
	set_int(name + ".num_diffuse_textures", material.texture_diffuses.size());

	for (int i = 0; i < material.speculars.size(); ++i, ++total_textures) {
		set_int(name + ".specular_textures[" + std::to_string(i) + "]", total_textures);
		set_int(name + ".specular_blend_strengths[" + std::to_string(i) + "]", material.speculars[i].blend_strength);
		set_int(name + ".specular_blend_ops[" + std::to_string(i) + "]", material.speculars[i].blend_op);
	}
	set_int(name + ".num_specular_textures", material.texture_speculars.size());

	set_vec3(name + ".diffuse_color", material.color_diffuse);

	set_vec3(name + ".specular_color", material.color_specular);

	set_float(name + ".shininess", material.shininess);

	set_float(name + ".opacity", material.opacity);
}

void Shader::set_light(const std::string& name, const Light& light) {
	set_vec3(name + ".ambient", light.ambient);
	set_vec3(name + ".diffuse", light.diffuse);
	set_vec3(name + ".specular", light.specular);
	set_float(name + ".constant", light.constant);
	set_float(name + ".linear", light.linear);
	set_float(name + ".quadratic", light.quadratic);
}
