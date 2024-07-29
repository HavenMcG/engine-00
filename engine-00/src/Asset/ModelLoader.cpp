#include "ModelLoader.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct ModelLoader::Private {
	static void process_node(Model* model, aiNode* node, const aiScene* scene, TextureManager& tl, MeshManager& mm);
	static std::pair<Mesh, Material> process_mesh(Model* model, aiMesh* mesh, const aiScene* scene, TextureManager& tl, MeshManager& mm);
	static std::vector<Texture> load_material_textures(Model* model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_tex_type, TextureManager& tl);
	static TextureType to_texture_type(aiTextureType ai_ttype);
};

TextureType ModelLoader::Private::to_texture_type(aiTextureType ai_ttype) {
	TextureType ttype;
	switch (ai_ttype) {
		case aiTextureType_NONE:
			ttype = NONE;
			break;
		case aiTextureType_DIFFUSE:
			ttype = Diffuse;
			break;
		case aiTextureType_SPECULAR:
			ttype = Specular;
			break;
		case aiTextureType_AMBIENT:
			ttype = Ambient;
			break;
		case aiTextureType_EMISSIVE:
			ttype = Emissive;
			break;
		case aiTextureType_HEIGHT:
			ttype = Heightmap;
			break;
		case aiTextureType_NORMALS:
			ttype = Normalmap;
			break;
		case aiTextureType_SHININESS:
			ttype = Shininess;
			break;
		case aiTextureType_OPACITY:
			ttype = Opacity;
			break;
		case aiTextureType_DISPLACEMENT:
			ttype = Displacement;
			break;
		case aiTextureType_LIGHTMAP:
			ttype = Lightmap;
			break;
		case aiTextureType_REFLECTION:
			ttype = Reflection;
			break;
		case aiTextureType_BASE_COLOR:
		case aiTextureType_NORMAL_CAMERA:
		case aiTextureType_EMISSION_COLOR:
		case aiTextureType_METALNESS:
		case aiTextureType_DIFFUSE_ROUGHNESS:
		case aiTextureType_AMBIENT_OCCLUSION:
		case aiTextureType_SHEEN:
		case aiTextureType_CLEARCOAT:
		case aiTextureType_TRANSMISSION:
		case aiTextureType_UNKNOWN:
		case _aiTextureType_Force32Bit:
		default:
			ttype = UNIMPLEMENTED;
			break;
	}
	return ttype;
}

void ModelLoader::read_model(Model* target, const std::string& path, TextureManager& tm, MeshManager& mm) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	}
	else {
		target->name = path;
		Private::process_node(target, scene->mRootNode, scene, tm, mm);
	}
}

void ModelLoader::Private::process_node(Model* model, aiNode* node, const aiScene* scene, TextureManager& tm, MeshManager& mm) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::pair<Mesh, Material> match = process_mesh(model, mesh, scene, tm, mm);
		model->meshes.push_back(match.first);
		model->materials.push_back(match.second);
	}
	// then do the same for each of it's children
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(model, node->mChildren[i], scene, tm, mm);
	}
}

std::pair<Mesh, Material> ModelLoader::Private::process_mesh(Model* model, aiMesh* mesh, const aiScene* scene, TextureManager& tm, MeshManager& mm) {
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

	Material new_mat{};
	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		new_mat.diffuses = load_material_textures(model, scene, material, aiTextureType_DIFFUSE, tm);
		new_mat.speculars = load_material_textures(model, scene, material, aiTextureType_SPECULAR, tm);
	}

	std::string mesh_name = model->name + "/" + mesh->mName.C_Str();
	Mesh new_mesh{ mesh_name };
	mm.load_data(new_mesh, vertices, indices);
	return { new_mesh, new_mat };
}

std::vector<Texture> ModelLoader::Private::load_material_textures(Model* model, const aiScene* scene, aiMaterial* mat, aiTextureType ai_ttype, TextureManager& tm) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(ai_ttype); ++i) {
		aiString path;
		mat->GetTexture(ai_ttype, i, &path);
		const aiTexture* ai_tex = scene->GetEmbeddedTexture(path.C_Str());
		TextureType ttype = Private::to_texture_type(ai_ttype);
		std::string full_path;
		bool embedded = (ai_tex != nullptr);
		if (embedded) {
			full_path = model->name + "/" + path.C_Str();
			bool compressed = (ai_tex->mHeight == 0);
			if (compressed) {
				tm.load_data_compressed(full_path, (unsigned char*)ai_tex->pcData, ai_tex->mWidth);
			}
			else {
				tm.load_data(full_path, ai_tex->pcData, RGBA, ai_tex->mWidth, ai_tex->mHeight);
			}
		}
		else {
			full_path = model->name.substr(0, model->name.find_last_of('/')) + "/" + path.C_Str();
			tm.load_file(full_path);
		}
		textures.push_back(Texture{ full_path });
	}
	return textures;
}
