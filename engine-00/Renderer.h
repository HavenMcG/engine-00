#pragma once
#include "Model.h"
#include "Shader.h"

class Renderer {
public:
	void draw_model(Model* model, Shader* shader);
	void draw_mesh(Mesh* mesh, Shader* shader);
};