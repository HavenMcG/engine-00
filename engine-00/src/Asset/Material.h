#pragma once
#include "Texture.h"
#include <vector>

struct TextureAssignment {
	Texture texture;
	float blend_strength;
};

struct Material {
	std::vector<TextureAssignment> diffuses;
	std::vector<TextureAssignment> speculars;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
	float shininess;
};