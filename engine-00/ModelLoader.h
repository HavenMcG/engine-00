#pragma once
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>

struct OGLMeshInfo {
	unsigned int vao, vbo, ebo;
};

class ModelLoader {
public:
	void read_model(Model* target, const std::string& path);

	void ogl_load_mesh(Mesh* target);
	OGLMeshInfo get_ogl_info(Mesh* mesh);
private:

	void process_node(Model* model, aiNode* node, const aiScene* scene);
	Mesh process_mesh(Model* model, aiMesh* mesh, const aiScene* scene);
	std::vector<std::string> load_material_textures(Model* model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_tex_type);
	
	static std::unordered_map<Mesh*, OGLMeshInfo> loaded_meshes; // the meshes loaded by OpenGL identified by their memory address mapped to their VAO id.
};

//Texture::Type convert_texture_type(aiTextureType ai_ttype);