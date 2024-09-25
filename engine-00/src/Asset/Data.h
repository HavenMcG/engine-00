#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
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

glm::vec3 tangent(std::vector<glm::vec3> pos, std::vector<glm::vec2> uv, glm::vec3 normal);

MeshData basic_quad_meshdata();

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