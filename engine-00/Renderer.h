#pragma once
#include "Model.h"
#include "Shader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

class Renderer {
public:
	void draw_model(Model* model, Shader* shader, ModelLoader& ml, TextureLoader& tl);
	void draw_mesh(Mesh* mesh, Shader* shader, ModelLoader& ml, TextureLoader& tl);
};