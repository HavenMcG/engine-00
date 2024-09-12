#pragma once
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Asset/Material.h"
#include "../HECS/Component/Light.h"
#include "../HECS/Component/Light.h"

const int MAX_TEXTURES_PER_STACK = 4;

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
	void set_material(const std::string& name, const Material& material);
	void set_light(std::string& name, const Light& light, glm::vec3 position);
	void set_directional_light(const std::string& name, const Light& light);

//private:
	// the program id
	unsigned int id_;
	int	num_point_lights_ = 0;
	int num_directional_lights_ = 0;
	int num_spotlights_ = 0;
};