#pragma once
#include <string>
#include "Mesh.h"
#include "Material.h"

struct Model {
	std::string name; // for models loaded from files, the name is the model's filepath
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};