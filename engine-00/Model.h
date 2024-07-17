#pragma once
#include <string>
#include "Mesh.h"

struct Model {
	std::vector<Mesh> meshes;
	std::string directory;
};