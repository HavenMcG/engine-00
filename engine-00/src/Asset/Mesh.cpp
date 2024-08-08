#include "Mesh.h"

bool operator==(const MeshInfo& lhs, const MeshInfo& rhs) {
	return lhs.path == rhs.path;
}

Mesh::Mesh(unsigned int index, unsigned char generation) {
	id = 0;
	id |= (index & MESH_INDEX_MASK);
	id |= (generation & MESH_GENERATION_MASK) << MESH_INDEX_BITS;
}

unsigned int Mesh::index() const {
	return id & MESH_INDEX_MASK;
}

unsigned char Mesh::generation() const {
	return (id >> MESH_INDEX_BITS) & MESH_GENERATION_MASK;
}

unsigned int Mesh::calc_id(unsigned int index, unsigned char generation) {
	unsigned int sim_id = 0;
	sim_id |= (index & MESH_INDEX_MASK);
	sim_id |= (generation & MESH_GENERATION_MASK) << MESH_INDEX_BITS;
	return sim_id;
}
