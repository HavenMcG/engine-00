#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <set>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

struct Mesh {
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::string> textures;
	Mesh();
	Mesh(std::string, std::vector<Vertex>, std::vector<unsigned int>, std::vector<std::string>);
private:
};