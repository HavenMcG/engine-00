#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "glad/glad.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

TextureManager::~TextureManager() {
	for (auto it = textures_loaded.begin(); it != textures_loaded.end(); ++it) {
		glDeleteTextures(1, &it->second);
	}
}

GLenum to_gl_enum(Format f) {
	switch (f) {
		case RED: return GL_RED;
		case RGB: return GL_RGB;
		case RGBA: return GL_RGBA;
	}
}

bool TextureManager::load(const Texture& texture) {
	bool status;
	bool already_loaded = ( textures_loaded.find(texture.path) != textures_loaded.end() );
	if (!already_loaded) {
		std::string name = texture.path.substr(texture.path.find_last_of('/') + 1, texture.path.size());
		bool embedded = (name[0] == '*');
		if (!embedded) {
			status = load_file(texture.path);
		}
		else {
			status = load_embedded(texture.path);
		}
	}
	else {
		status = false;
	}
	return status;
}

bool TextureManager::unload(const Texture& texture) {
	auto r = textures_loaded.find(texture.path);
	if (r == textures_loaded.end()) {
		return false;
	}
	else {
		std::cout << "unloading texture " << texture.path << std::endl;
		unsigned int tex_id = r->second;
		glDeleteTextures(1, &tex_id);
		textures_loaded.erase(texture.path);
		return true;
	}
}

bool TextureManager::load_file(const std::string& path) {
	bool status;
	int prior_flip_setting = stbi__vertically_flip_on_load_global;
	//stbi_set_flip_vertically_on_load(true);
	int width, height, nr_components;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);
	if (data) {
		Format format;
		if (nr_components == 1) format = RED;
		else if (nr_components == 3) format = RGB;
		else format = RGBA;

		status = load_data(path, data, format, width, height);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		status = false;
	}
	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(prior_flip_setting);
	return status;
}

bool TextureManager::load_data_compressed(const std::string& path, const void* compressed_data, unsigned int size) {
	bool status;
	int width, height, num_channels;
	void* data = stbi_load_from_memory((unsigned char*)compressed_data, size, &width, &height, &num_channels, 0);
	if (data) {
		Format format;
		if (num_channels == 1) format = RED;
		else if (num_channels == 3) format = RGB;
		else format = RGBA;

		status = load_data(path, data, format, width, height);
	}
	else {
		std::cout << "Compressed embedded texture failed to load." << std::endl;
		status = false;
	}
	return status;
}

bool TextureManager::load_data(const std::string& path, const void* data, Format format, int width, int height) {
	bool status = true; // WIP
	unsigned int tex_id;
	glGenTextures(1, &tex_id);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, to_gl_enum(format), width, height, 0, to_gl_enum(format), GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::cout << "loading texture " << path << std::endl;
	textures_loaded.emplace(path, tex_id);
	return status;
}

bool TextureManager::load_embedded(const std::string& path) {
	bool status;
	std::string texture_name = path.substr(path.find_last_of('/') + 1, path.size());
	std::string filepath = path.substr(0, path.find_last_of('/'));
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return false;
	}
	else {
		const aiTexture* texture = scene->GetEmbeddedTexture(texture_name.c_str());
		bool compressed = ( texture->mHeight == 0 );
		if (compressed) {
			status = load_data_compressed(path, texture->pcData, texture->mWidth);
		}
		else {
			status = load_data(path, texture->pcData, RGBA, texture->mWidth, texture->mHeight);
		}
	}
	return status;
}

bool TextureManager::id(const std::string& path, unsigned int& id) {
	auto r = textures_loaded.find(path);
	if (r == textures_loaded.end()) return false;
	else {
		id = r->second;
		return true;
	}
}