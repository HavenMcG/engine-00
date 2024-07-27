#include "Mesh.h"
#include "iostream"

Mesh::Mesh() {
	vertices = {};
	indices = {};
	textures = {};
	std::cout << "mesh()" << std::endl;
}

Mesh::Mesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::string> textures) 
		: name{ name }
		, vertices{ vertices }
		, indices{ indices }
		, textures{ textures } {
	std::cout << "mesh(path, vertices, indices, textures)" << std::endl;
}