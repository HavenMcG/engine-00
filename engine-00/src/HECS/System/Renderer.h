#pragma once
#include "../Component/Transform.h"
#include "../Component/Model.h"
#include "../Component/Light.h"
#include "../../engine-00/Shader.h"
#include "../../Asset/Store/OpenGL/OglAssetStore.h"

class Renderer {
public:
	void draw_models(glm::mat4 view_matrix, Shader& shader, ModelCollection& models, TransformCollection& transforms, LightCollection& lights, const OglAssetStore& assets);
	void draw_ui(Shader& shader, ModelCollection& models, TransformCollection& transforms, const OglAssetStore& assets);
};