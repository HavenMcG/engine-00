#include "ModelLoader.h"
#include <iostream>
#include "TextureLoader.h"
#include "glad/glad.h"

std::unordered_map<Mesh*, OGLMeshInfo> ModelLoader::loaded_meshes = {};

void ModelLoader::read_model(Model* target, const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	}
	else {
		target->directory = path.substr(0, path.find_last_of('/'));
		process_node(target, scene->mRootNode, scene);
	}
}

void ModelLoader::ogl_load_mesh(Mesh* target) {
	bool already_loaded = ( loaded_meshes.find(target) != loaded_meshes.end() );
	if (!already_loaded) {
		unsigned int vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, target->vertices.size() * sizeof(Vertex), target->vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), target->indices.data(), GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		loaded_meshes.insert(std::pair<Mesh*, OGLMeshInfo>{ target, OGLMeshInfo{ vao,vbo,ebo } });
	}
}

OGLMeshInfo ModelLoader::get_ogl_info(Mesh* mesh) {
	auto result = loaded_meshes.find(mesh);
	if (result != loaded_meshes.end()) return result->second;
	else return OGLMeshInfo{};
}

void ModelLoader::process_node(Model* model, aiNode* node, const aiScene* scene) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model->meshes.push_back(process_mesh(model, mesh, scene));
	}
	// then do the same for each of it's children
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(model, node->mChildren[i], scene);
	}
}

Mesh ModelLoader::process_mesh(Model* model, aiMesh* mesh, const aiScene* scene) {
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
		std::vector<std::string> diffuse_maps = load_material_textures(model, scene, material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		std::vector<std::string> specular_maps = load_material_textures(model, scene, material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh{ vertices, indices, textures };
}

std::vector<std::string> ModelLoader::load_material_textures(Model* model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_ttype) {
	std::vector<std::string> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(ai_ttype); ++i) {
		aiString path;
		mat->GetTexture(ai_ttype, i, &path);
		const aiTexture* ai_tex = scene->GetEmbeddedTexture(path.C_Str());
		TextureType ttype = convert_texture_type(ai_ttype);
		TextureLoader tl;
		std::string texture = model->directory + '/' + path.C_Str();
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
