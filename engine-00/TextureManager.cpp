#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "glad/glad.h"

GLenum to_gl_enum(Format f) {
	switch (f) {
		case RED: return GL_RED;
		case RGB: return GL_RGB;
		case RGBA: return GL_RGBA;
	}
}

unsigned int TextureManager::load_file(const std::string& path) {

	bool already_loaded = ( textures_loaded.find(path) != textures_loaded.end() );
	unsigned int tex_id = 0;

	if (!already_loaded) {
		int prior_flip_setting = stbi__vertically_flip_on_load_global;
		//stbi_set_flip_vertically_on_load(true);

		int width, height, nr_components;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);

		if (data) {
			Format format;
			if (nr_components == 1) format = RED;
			else if (nr_components == 3) format = RGB;
			else format = RGBA;

			tex_id = load_data(path, data, format, width, height);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
		}

		stbi_image_free(data);
		stbi_set_flip_vertically_on_load(prior_flip_setting);
	}
	else {
		tex_id = textures_loaded.find(path)->second;
	}
	return tex_id;
}

unsigned int TextureManager::load_data_compressed(const std::string& path, unsigned char* compressed_data, unsigned int size) {
	bool already_loaded = (textures_loaded.find(path) != textures_loaded.end());
	unsigned int tex_id = 0;
	if (!already_loaded) {
		int width, height, num_channels;
		void* data = stbi_load_from_memory(compressed_data, size, &width, &height, &num_channels, 0);
		if (data) {
			Format format;
			if (num_channels == 1) format = RED;
			else if (num_channels == 3) format = RGB;
			else format = RGBA;

			tex_id = load_data(path, data, format, width, height);
		}
		else {
			std::cout << "Compressed embedded texture failed to load." << std::endl;
		}
	}
	else {
		tex_id = textures_loaded.find(path)->second;
	}
	return tex_id;
}

unsigned int TextureManager::load_data(const std::string& path, const void* data, Format format, int width, int height) {
	unsigned int tex_id;
	glGenTextures(1, &tex_id);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, to_gl_enum(format), width, height, 0, to_gl_enum(format), GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	textures_loaded.emplace(path, tex_id);
	return tex_id;
}

bool TextureManager::id(const std::string& path, unsigned int& id) {
	auto r = textures_loaded.find(path);
	if (r == textures_loaded.end()) return false;
	else {
		id = r->second;
		return true;
	}
}