#pragma once
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Asset/Material.h"
#include "../Asset/Store/OpenGL/OglAssetStore.h"
#include "../engine-00/Light.h"

class Shader {
public:
	// constructor reads and builds the shader
	Shader(const char* vertex_path, const char* fragment_path);
	~Shader();
	unsigned int id() const;
	// use/activate the shader
	void use();
	// utility uniform functions
	void set_bool(const std::string& name, bool value);
	void set_int(const std::string& name, int value);
	void set_float(const std::string& name, float value);
	void set_vec2(const std::string& name, const glm::vec2& value);
	void set_vec2(const std::string& name, float x, float y);
	void set_vec3(const std::string& name, const glm::vec3& value);
	void set_vec3(const std::string& name, float x, float y, float z);
	void set_vec4(const std::string& name, const glm::vec4& value);
	void set_vec4(const std::string& name, float x, float y, float z, float w);
	void set_mat2(const std::string& name, const glm::mat2& mat);
	void set_mat3(const std::string& name, const glm::mat3& mat);
	void set_mat3x2(const std::string& name, const glm::mat3x2& mat);
	void set_mat4(const std::string& name, const glm::mat4& mat);
	void set_material(const std::string& name, const Material& material, const OglAssetStore& texture_m);
	void set_light(const std::string& name, const Light& light);

private:
	// the program id
	unsigned int id_;
};