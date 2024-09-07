#pragma once
#include "Texture.h"
#include <vector>

// these need to match constants in the shader
enum TextureBlendOp {
	Add = 0,
	Multiply = 1
};

struct TextureAssignment {
	Texture texture;
	float blend_strength;
	TextureBlendOp blend_op;
};

struct Material {
	std::vector<TextureAssignment> diffuses;
	std::vector<TextureAssignment> speculars;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
	float shininess;
	float opacity = 1.0f;
};