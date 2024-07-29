#pragma once
#include "Model.h"
#include "TextureManager.h"
#include "MeshManager.h"

class ModelLoader {
public:
	void read_model(Model* target, const std::string& path, TextureManager& tm, MeshManager& mm);
private:
	struct Private;
};