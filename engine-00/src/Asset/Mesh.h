#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <set>

struct Mesh {
	// The unique identifier of the mesh.
	std::string path;
	size_t num_indices;
};