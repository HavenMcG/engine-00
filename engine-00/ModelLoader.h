#pragma once
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <unordered_set>
#include "TextureManager.h"

class ModelLoader {
public:
	void read_model(Model* target, const std::string& path, TextureManager& tl);
private:
	void process_node(Model* model, aiNode* node, const aiScene* scene, TextureManager& tl);
	Mesh process_mesh(Model* model, aiMesh* mesh, const aiScene* scene, TextureManager& tl);
	std::vector<std::string> load_material_textures(Model* model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_tex_type, TextureManager& tl);
};