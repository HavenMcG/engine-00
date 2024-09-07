#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

struct Cuboid {
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 size();
	MeshData generate_mesh();
	glm::vec3 center();
};

const MeshData BASIC_QUAD_MESH_DATA {
	std::vector<Vertex> {
		{ { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
	},
	std::vector<unsigned int> {
		0, 1, 3,
		1, 2, 3
	}
};

enum TextureType {
	Diffuse,
	Specular,
	Ambient,
	Emissive,
	Heightmap,
	Normalmap,
	Shininess,
	Opacity,
	Displacement,
	Lightmap,
	Reflection,
	UNIMPLEMENTED,
	NONE
};

enum Format {
	RED,
	RGB,
	RGBA
};

struct TextureData {
	const void* data;
	Format format;
	int width;
	int height;
};

Cuboid generate_bounding_box(const MeshData& data);