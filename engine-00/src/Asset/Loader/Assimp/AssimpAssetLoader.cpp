#include "AssimpAssetLoader.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct AssimpAssetLoader::Private {
	static void process_node(Model& model, aiNode* node, const aiScene* scene, AssetStore& store);
	static std::pair<Mesh, Material> process_mesh(Model& model, aiMesh* mesh, const aiScene* scene, AssetStore& store);
	static std::vector<TextureAssignment> load_material_textures(Model& model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_tex_type, AssetStore& store);
	static std::expected<Texture, ErrorCode>  load_file(const std::string& path, AssetStore& store);
	static std::expected<Texture, ErrorCode> load_compressed(const std::string& path, AssetStore& store, const void* compressed_data, unsigned int size);
	static TextureBlendOp to_blend_op(aiTextureOp in);
};

std::expected<Model, bool> AssimpAssetLoader::load_model(const std::string& path, AssetStore& store) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return std::unexpected(false);
	}
	else {
		Model m{};
		m.name = path;
		Private::process_node(m, scene->mRootNode, scene, store);
		return m;
	}
}

void AssimpAssetLoader::Private::process_node(Model& model, aiNode* node, const aiScene* scene, AssetStore& store) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::pair<Mesh, Material> match = process_mesh(model, mesh, scene, store);
		model.meshes.push_back(match.first);
		model.materials.push_back(match.second);
	}
	// then do the same for each of it's children
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(model, node->mChildren[i], scene, store);
	}
}

std::pair<Mesh, Material> AssimpAssetLoader::Private::process_mesh(Model& model, aiMesh* ai_mesh, const aiScene* scene, AssetStore& store) {
	Mesh mesh{ 0,0 };
	std::string mesh_path = model.name + "/" + ai_mesh->mName.C_Str();
	auto check = store.mesh(mesh_path);
	if (check.has_value()) mesh = *check;
	else {
		MeshData mesh_data{};

		// process vertices
		for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
			Vertex vertex;
			vertex.position = {
				ai_mesh->mVertices[i].x,
				ai_mesh->mVertices[i].y,
				ai_mesh->mVertices[i].z
			};
			vertex.normal = {
				ai_mesh->mNormals[i].x,
				ai_mesh->mNormals[i].y,
				ai_mesh->mNormals[i].z
			};

			if (ai_mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
				glm::vec2 tex_coords{
					ai_mesh->mTextureCoords[0][i].x,
					ai_mesh->mTextureCoords[0][i].y
				};
				vertex.tex_coords = tex_coords;
			}
			else {
				vertex.tex_coords = glm::vec2{ 0.0f, 0.0f };
			}

			mesh_data.vertices.push_back(vertex);
		}

		// process indices
		for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
			aiFace face = ai_mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				mesh_data.indices.push_back(face.mIndices[j]);
			}
		}

		// load mesh
		mesh = *store.load(mesh_data, mesh_path);
	}

	// process material
	Material material{};
	if (ai_mesh->mMaterialIndex >= 0) {
		aiMaterial* ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];

		// process textures
		material.texture_diffuses = load_material_textures(model, scene, ai_material, aiTextureType_DIFFUSE, store);
		material.texture_speculars = load_material_textures(model, scene, ai_material, aiTextureType_SPECULAR, store);
		material.texture_emissives = load_material_textures(model, scene, ai_material, aiTextureType_EMISSIVE, store);

		// process other fields
		aiColor3D ai_color_diffuse;
		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color_diffuse);
		material.color_diffuse = { ai_color_diffuse.r, ai_color_diffuse.g, ai_color_diffuse.b };

		aiColor3D ai_color_specular;
		ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_color_specular);
		material.color_specular = { ai_color_specular.r, ai_color_specular.g, ai_color_specular.b };

		aiColor3D ai_color_emissive;
		ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, ai_color_emissive);
		material.color_emissive = { ai_color_emissive.r, ai_color_emissive.g, ai_color_emissive.b };

		float shininess;
		ai_material->Get(AI_MATKEY_SHININESS, shininess);
		material.shininess = shininess;
	}

	return { mesh, material };
}

std::vector<TextureAssignment> AssimpAssetLoader::Private::load_material_textures(Model& model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_ttype, AssetStore& store) {
	std::vector<TextureAssignment> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(ai_ttype); ++i) {
		aiString ai_path;
		mat->GetTexture(ai_ttype, i, &ai_path);
		const aiTexture* ai_tex = scene->GetEmbeddedTexture(ai_path.C_Str());
		std::string path;
		Texture texture{0,0};
		bool embedded = (ai_tex != nullptr);
		if (embedded) {
			path = model.name + "/" + ai_path.C_Str();
			auto check = store.texture(path);
			if (check.has_value()) texture = *check;
			else {
				bool compressed = (ai_tex->mHeight == 0);
				if (compressed) {
					texture = *load_compressed(path, store, (unsigned char*)ai_tex->pcData, ai_tex->mWidth);
				}
				else {
					texture = *store.load(TextureData{ ai_tex->pcData, RGBA, static_cast<int>(ai_tex->mWidth), static_cast<int>(ai_tex->mHeight) }, path);
				}
			}
		}
		else {
			path = model.name.substr(0, model.name.find_last_of('/')) + "/" + ai_path.C_Str();
			auto check = store.texture(path);
			if (check.has_value()) texture = *check;
			else {
				texture = *load_file(path, store);
			}
		}

		// get other texture assignment properties
		float blend_strength = 1.0f;
		mat->Get(AI_MATKEY_TEXBLEND(ai_ttype, i), blend_strength);

		aiTextureOp ai_blend_op;
		mat->Get(AI_MATKEY_TEXOP(ai_ttype, i), ai_blend_op);

		TextureAssignment ta{ 
			texture, 
			blend_strength, 
			to_blend_op(ai_blend_op) 
		};
		textures.push_back(ta);
	}
	return textures;
}

std::expected<Texture, ErrorCode>  AssimpAssetLoader::Private::load_file(const std::string& path, AssetStore& store) {
	int prior_flip_setting = stbi__vertically_flip_on_load_global;
	//stbi_set_flip_vertically_on_load(true);
	int width, height, nr_components;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);
	bool succeeded = false;
	Texture texture{ 0,0 };
	ErrorCode error;
	if (data) {
		Format format;
		if (nr_components == 1) format = RED;
		else if (nr_components == 3) format = RGB;
		else format = RGBA;

		auto result = store.load(TextureData{ data, format, width, height }, path);
		if (result.has_value()) {
			texture = *result;
			succeeded = true;
		}
		else error = result.error();
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(prior_flip_setting);
	if (succeeded) return texture;
	else return std::unexpected(error);
}

std::expected<Texture, ErrorCode>  AssimpAssetLoader::Private::load_compressed(const std::string& path, AssetStore& store, const void* compressed_data, unsigned int size) {
	int width, height, num_channels;
	void* data = stbi_load_from_memory((unsigned char*)compressed_data, size, &width, &height, &num_channels, 0);
	bool succeeded = false;
	Texture texture{ 0,0 };
	ErrorCode error;
	if (data) {
		Format format;
		if (num_channels == 1) format = RED;
		else if (num_channels == 3) format = RGB;
		else format = RGBA;

		auto r = store.load(TextureData{ data, format, width, height }, path);
		if (r.has_value()) {
			texture = *r;
			succeeded = true;
		}
		else error = r.error();
	}
	else {
		std::cout << "Compressed embedded texture failed to load." << std::endl;
	}
	stbi_image_free(data);
	if (succeeded) return texture;
	else return std::unexpected(error);
}

TextureBlendOp AssimpAssetLoader::Private::to_blend_op(aiTextureOp in) {
	switch (in) {
		case aiTextureOp_Multiply: return Multiply; break;
		case aiTextureOp_Add: return Add; break;
		case aiTextureOp_Subtract:
		case aiTextureOp_Divide:
		case aiTextureOp_SmoothAdd:
		case aiTextureOp_SignedAdd:
		case _aiTextureOp_Force32Bit:
		default:
			return Multiply;
			break;
	}
}