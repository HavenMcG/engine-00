#pragma once
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <unordered_set>
#include "TextureLoader.h"

struct OGLMeshInfo {
	unsigned int vao, vbo, ebo;
};

class ModelLoader {
public:
	void read_model(Model* target, const std::string& path, TextureLoader tl);

	void ogl_load_mesh(Mesh* target);
	OGLMeshInfo get_ogl_info(Mesh* mesh);
private:

	void process_node(Model* model, aiNode* node, const aiScene* scene, TextureLoader& tl);
	Mesh process_mesh(Model* model, aiMesh* mesh, const aiScene* scene, TextureLoader& tl);
	std::vector<std::string> load_material_textures(Model* model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_tex_type, TextureLoader& tl);
	
	std::unordered_map<unsigned int, size_t> loaded_meshes; // the meshes loaded by OpenGL identified by their memory address mapped to their index in the container.
	std::vector<OGLMeshInfo> meshes;
};

//Texture::Type convert_texture_type(aiTextureType ai_ttype);