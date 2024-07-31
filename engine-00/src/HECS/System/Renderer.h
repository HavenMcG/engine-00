#pragma once
#include "../Component/Transform3dManager.h"
#include "../Component/ModelManager.h"
#include "../../engine-00/Shader.h"
#include "../../Asset/MeshManager.h"
#include "../../Asset/TextureManager.h"

class Renderer {
public:
	void draw_models(ModelManager& model_m, Transform3dManager& transform_m, MeshManager& mesh_m, TextureManager& texture_m, Shader& shader);
};