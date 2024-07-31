#pragma once
#include "../Asset/Model.h"
#include "Shader.h"
#include "../Asset/TextureManager.h"
#include "../Asset/MeshManager.h"

class SimpleRenderer {
public:
	void draw_model(Model* model, Shader* shader, MeshManager& mm, TextureManager& tl);
	void draw_mesh(const Mesh& mesh, const Material& material, Shader* shader, MeshManager& mm, TextureManager& tl);
};