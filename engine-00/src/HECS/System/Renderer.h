#pragma once
#include "../Component/Transform.h"
#include "../Component/Model.h"
#include "../../engine-00/Shader.h"

class Renderer {
public:
	void draw_models(glm::mat4 view_matrix, Shader& shader, ModelCollection& model_m, TransformCollection& transform_m, const OglAssetStore& assets);
};