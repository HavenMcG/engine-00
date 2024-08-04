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
	static bool load_file(Texture& texture, AssetStore& store);
	static bool load_compressed(Texture& texture, AssetStore& store, const void* compressed_data, unsigned int size);
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
	std::string mesh_name = model.name + "/" + ai_mesh->mName.C_Str();
	Mesh mesh{ mesh_name };
	std::vector<std::string> textures;
	if (!store.loaded(mesh)) {
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
		mesh.num_indices = mesh_data.indices.size();

		// load mesh
		store.load(mesh, mesh_data);
	}
	else {
		mesh = *store.key(mesh);
	}

	// process material
	Material material{};
	if (ai_mesh->mMaterialIndex >= 0) {
		aiMaterial* ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];

		// process textures
		material.diffuses = load_material_textures(model, scene, ai_material, aiTextureType_DIFFUSE, store);
		material.speculars = load_material_textures(model, scene, ai_material, aiTextureType_SPECULAR, store);

		// process other fields
		aiColor3D ai_color_diffuse;
		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color_diffuse);
		material.color_diffuse = { ai_color_diffuse.r, ai_color_diffuse.g, ai_color_diffuse.b };

		aiColor3D ai_color_specular;
		ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_color_specular);
		material.color_specular = { ai_color_specular.r, ai_color_specular.g, ai_color_specular.b };

		float shininess;
		ai_material->Get(AI_MATKEY_SHININESS, shininess);
		material.shininess = shininess;
	}

	return { mesh, material };
}

std::vector<TextureAssignment> AssimpAssetLoader::Private::load_material_textures(Model& model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_ttype, AssetStore& store) {
	std::vector<TextureAssignment> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(ai_ttype); ++i) {
		aiString path;
		mat->GetTexture(ai_ttype, i, &path);
		const aiTexture* ai_tex = scene->GetEmbeddedTexture(path.C_Str());
		Texture texture{};
		bool embedded = (ai_tex != nullptr);
		if (embedded) {
			texture.path = model.name + "/" + path.C_Str();
			bool compressed = (ai_tex->mHeight == 0);
			if (compressed) {
				load_compressed(texture, store, (unsigned char*)ai_tex->pcData, ai_tex->mWidth);
			}
			else {
				store.load(texture, TextureData{ ai_tex->pcData, RGBA, static_cast<int>(ai_tex->mWidth), static_cast<int>(ai_tex->mHeight) });
			}
		}
		else {
			texture.path = model.name.substr(0, model.name.find_last_of('/')) + "/" + path.C_Str();
			load_file(texture, store);
		}

		// get other texture assignment properties
		float blend_strength = 1.0f;
		mat->Get(AI_MATKEY_TEXBLEND(ai_ttype, i), blend_strength);

		aiTextureOp ai_blend_op = aiTextureOp_Multiply;
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

bool AssimpAssetLoader::Private::load_file(Texture& texture, AssetStore& store) {
	bool status;
	int prior_flip_setting = stbi__vertically_flip_on_load_global;
	//stbi_set_flip_vertically_on_load(true);
	int width, height, nr_components;
	unsigned char* data = stbi_load(texture.path.c_str(), &width, &height, &nr_components, 0);
	if (data) {
		Format format;
		if (nr_components == 1) format = RED;
		else if (nr_components == 3) format = RGB;
		else format = RGBA;

		store.load(texture, TextureData{ data, format, width, height });
		status = true;
	}
	else {
		std::cout << "Texture failed to load at path: " << texture.path << std::endl;
		status = false;
	}
	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(prior_flip_setting);
	return status;
}

bool AssimpAssetLoader::Private::load_compressed(Texture& texture, AssetStore& store, const void* compressed_data, unsigned int size) {
	bool status;
	int width, height, num_channels;
	void* data = stbi_load_from_memory((unsigned char*)compressed_data, size, &width, &height, &num_channels, 0);
	if (data) {
		Format format;
		if (num_channels == 1) format = RED;
		else if (num_channels == 3) format = RGB;
		else format = RGBA;

		store.load(texture, TextureData{ data, format, width, height });
		status = true;
	}
	else {
		std::cout << "Compressed embedded texture failed to load." << std::endl;
		status = false;
	}
	stbi_image_free(data);
	return status;
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