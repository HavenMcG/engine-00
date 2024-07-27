#pragma once
#include "Model.h"
#include "Shader.h"
#include "TextureManager.h"
#include "MeshManager.h"

class Renderer {
public:
	void draw_model(Model* model, Shader* shader, MeshManager& mm, TextureManager& tl);
	void draw_mesh(Mesh* mesh, Shader* shader, MeshManager& mm, TextureManager& tl);
};