#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "glad/glad.h"

TextureType convert_texture_type(aiTextureType ai_ttype) {
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

unsigned int TextureManager::load_texture_file(const std::string& path) {

	bool already_loaded = ( textures_loaded.find(path) != textures_loaded.end() );
	unsigned int tex_id = 0;

	if (!already_loaded) {
		int prior_flip_setting = stbi__vertically_flip_on_load_global;
		//stbi_set_flip_vertically_on_load(true);

		int width, height, nr_components;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);

		if (data) {
			GLenum format = 0;
			if (nr_components == 1) format = GL_RED;
			else if (nr_components == 3) format = GL_RGB;
			else if (nr_components == 4) format = GL_RGBA;

			tex_id = load_texture_data(data, format, width, height);
			textures_loaded.emplace(path, tex_id);
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

unsigned int TextureManager::load_embedded_texture(const aiTexture* ai_tex, const std::string& path) {
	bool already_loaded = (textures_loaded.find(path) != textures_loaded.end());
	unsigned int tex_id = 0;
	if (!already_loaded) {
		aiTexel* ai_data = ai_tex->pcData;
		GLenum format = GL_RGBA;
		auto ai_width = ai_tex->mWidth;
		auto ai_height = ai_tex->mHeight;

		int width, height, num_channels;
		void* data;

		if (ai_height == 0) { // data is compressed in a format like png
			data = stbi_load_from_memory((unsigned char*)ai_data, ai_tex->mWidth, &width, &height, &num_channels, 0);
		}
		else {
			data = ai_data;
			width = ai_width;
			height = ai_height;
			num_channels = 4;
		}

		if (data) {
			GLenum format = 0;
			if (num_channels == 1) format = GL_RED;
			else if (num_channels == 3) format = GL_RGB;
			else if (num_channels == 4) format = GL_RGBA;

			tex_id = load_texture_data(data, format, width, height);
			textures_loaded.emplace(path, tex_id);
		}
		else {
			std::cout << "Embedded texture failed to load. Info:" << std::endl;
			std::cout << "Compressed: ";
			if (ai_height == 0) std::cout << "Yes" << std::endl;
			else std::cout << "No" << std::endl;
			std::cout << "Format Hint: " << ai_tex->achFormatHint << std::endl;
		}
	}
	else {
		tex_id = textures_loaded.find(path)->second;
	}
	return tex_id;
}

unsigned int TextureManager::load_texture_data(const void* data, unsigned int format, int width, int height) {
	unsigned int tex_id;
	glGenTextures(1, &tex_id);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex_id;
}

bool TextureManager::find_id(const std::string& path, unsigned int& id) {
	auto r = textures_loaded.find(path);
	if (r == textures_loaded.end()) return false;
	else {
		id = r->second;
		return true;
	}
}

unsigned int load_tex_test(const std::string& path) {
	std::string directory = path.substr(0, path.find_last_of('/'));
	std::string filename = path.substr(path.find_last_of('/')+1, path.size()-1);
	bool embedded = ( filename[0] == '*' );
	//std::cout << "dir:  " << directory << std::endl << "file: " << filename << std::endl;
	return 0;
}