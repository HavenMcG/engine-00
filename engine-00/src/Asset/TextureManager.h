#pragma once
#include "Texture.h"
#include <unordered_map>

enum TextureType {
	Diffuse,
	Specular,
	Ambient,
	Emissive,
	Heightmap,
	Normalmap,
	Shininess,
	Opacity,
	Displacement,
	Lightmap,
	Reflection,
	UNIMPLEMENTED,
	NONE
};

enum Format {
	RED,
	RGB,
	RGBA
};

class TextureManager {
public:
	unsigned int load_file(const std::string& path);
	unsigned int load_data_compressed(const std::string& path, const void* data, unsigned int size);
	unsigned int load_data(const std::string& path, const void* data, Format format, int width, int height);
	bool id(const std::string& path, unsigned int& id);

	TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator= (const TextureManager&) = delete;
	~TextureManager() = default;

private:
	std::unordered_map<std::string, unsigned int> textures_loaded; // The textures loaded by OpenGL identified by their filepath mapped to their OpenGL id.
};
