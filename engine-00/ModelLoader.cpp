#include "ModelLoader.h"
#include <iostream>
#include "TextureManager.h"
#include "glad/glad.h"

void ModelLoader::read_model(Model* target, const std::string& path, TextureManager& tl) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	}
	else {
		target->name = path;
		process_node(target, scene->mRootNode, scene, tl);
	}
}

void ModelLoader::process_node(Model* model, aiNode* node, const aiScene* scene, TextureManager& tl) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model->meshes.push_back(process_mesh(model, mesh, scene, tl));
	}
	// then do the same for each of it's children
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(model, node->mChildren[i], scene, tl);
	}
}

Mesh ModelLoader::process_mesh(Model* model, aiMesh* mesh, const aiScene* scene, TextureManager& tl) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::string> textures;
	// process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		vertex.position = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};
		vertex.normal = {
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		};

		if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
			glm::vec2 tex_coords{
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			};
			vertex.tex_coords = tex_coords;
		}
		else {
			vertex.tex_coords = glm::vec2{ 0.0f, 0.0f };
		}
		
		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<std::string> diffuse_maps = load_material_textures(model, scene, material, aiTextureType_DIFFUSE, tl);
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		std::vector<std::string> specular_maps = load_material_textures(model, scene, material, aiTextureType_SPECULAR, tl);
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh{ model->name + "/" + mesh->mName.C_Str(), vertices, indices, textures};
}

std::vector<std::string> ModelLoader::load_material_textures(Model* model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_ttype, TextureManager& tl) {
	std::vector<std::string> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(ai_ttype); ++i) {
		aiString path;
		mat->GetTexture(ai_ttype, i, &path);
		const aiTexture* ai_tex = scene->GetEmbeddedTexture(path.C_Str());
		TextureType ttype = convert_texture_type(ai_ttype);
		std::string texture = model->name.substr(0, model->name.find_last_of('/') + 1) + path.C_Str();
		bool embedded = (ai_tex != nullptr);
		if (embedded) {
			tl.load_embedded_texture(ai_tex, texture);
		}
		else {
			tl.load_texture_file(texture);
		}
		textures.push_back(texture);
	}
	return textures;
}
