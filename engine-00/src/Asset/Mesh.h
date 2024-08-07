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

struct Mesh {
	unsigned int id;
	Mesh(unsigned int index, unsigned char generation);
	unsigned int index() const;
	unsigned char generation() const;
};

struct MeshInfo {
	// The unique identifier of the mesh. Used to compare meshes - same path means they are considered equal.
	// If you are not loading from disk you can make the path whatever you'd like.
	std::string path;
};

bool operator==(const MeshInfo& lhs, const MeshInfo& rhs);

// To hash a mesh we simply hash it's path
template<> struct std::hash<MeshInfo> {
	std::size_t operator()(const MeshInfo& mesh_info) const {
		return std::hash<std::string>{}(mesh_info.path);
	}
};