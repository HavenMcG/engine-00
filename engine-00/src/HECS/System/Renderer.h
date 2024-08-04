#pragma once
#include "../Component/Transform3dManager.h"
#include "../Component/ModelManager.h"
#include "../../engine-00/Shader.h"

class Renderer {
public:
	void draw_models(glm::mat4 view_matrix, Shader& shader, ModelManager& model_m, Transform3dManager& transform_m, const OglAssetStore& assets);
};