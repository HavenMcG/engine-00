#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <set>

const unsigned int MESH_INDEX_BITS = 24;
const unsigned int MESH_INDEX_MASK = (1 << MESH_INDEX_BITS) - 1;

const unsigned int MESH_GENERATION_BITS = 8;
const unsigned int MESH_GENERATION_MASK = (1 << MESH_INDEX_BITS) - 1;

// We've split up our 30 bits into 22 bits for the index and 8 bits for the generation.
// This means that we support a maximum of 4 million simultaneous entities.
// It also means that we can only distinguish between 256 different entities created at the same index slot.
// If more than 256 entities are created at the same index slot, the generation value will wrap around
// and our new entity will get the same ID as an old entity.

struct Mesh {
	unsigned int id;
	Mesh(unsigned int index, unsigned char generation);
	unsigned int index() const;
	unsigned char generation() const;
	static unsigned int calc_id(unsigned int index, unsigned char generation);
};