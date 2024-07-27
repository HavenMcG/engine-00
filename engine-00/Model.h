#pragma once
#include <string>
#include "Mesh.h"

struct Model {
	std::string name; // for models loaded from files, the name is the model's filepath
	std::vector<Mesh> meshes;
};