#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <set>

struct Mesh {
	// The unique identifier of the mesh. Used to compare meshes - same path means they are considered equal.
	// If you are not loading from disk you can make the path whatever you'd like.
	std::string path;
	size_t num_indices;
};

bool operator==(const Mesh& lhs, const Mesh& rhs);

// To hash a mesh we simply hash it's path
template<> struct std::hash<Mesh> {
	std::size_t operator()(const Mesh& mesh) const {
		return std::hash<std::string>{}(mesh.path);
	}
};