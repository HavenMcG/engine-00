#include "Mesh.h"
#include "iostream"

unsigned int Mesh::next_id_ = 0;
std::set<unsigned int> Mesh::free_ids_ = {};

Mesh::Mesh() {
	id_ = claim_id();
	vertices = {};
	indices = {};
	textures = {};
	std::cout << "mesh()" << std::endl;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::string> textures) 
		: vertices{ vertices }
		, indices{ indices }
		, textures{ textures } {
	id_ = claim_id();
	std::cout << "mesh(vertices, indices, textures)" << std::endl;
}

Mesh::~Mesh() {
	free_id(id_);
}

unsigned int Mesh::id() {
	return id_;
}

unsigned int Mesh::claim_id() {
	unsigned int id;
	if (!free_ids_.empty()) {
		id = *free_ids_.begin();
		free_ids_.erase(id);
	}
	else id = next_id_++;
	return id;
}

void Mesh::free_id(unsigned int tid) {
	if (tid == next_id_ - 1) --next_id_;
	else free_ids_.insert(tid);
	while (free_ids_.size() > 0 && *std::prev(free_ids_.end()) > next_id_) {
		free_ids_.erase(*std::prev(free_ids_.end()));
	}
}
