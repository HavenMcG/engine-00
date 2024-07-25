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
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::string> textures;
	Mesh();
	Mesh(std::vector<Vertex>, std::vector<unsigned int>, std::vector<std::string>);
	~Mesh();
	unsigned int id();
private:
	unsigned int id_;
	static unsigned int next_id_;
	static std::set<unsigned int> free_ids_;
	unsigned int claim_id();
	void free_id(unsigned int);
};