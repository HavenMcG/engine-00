#pragma once
#include "Texture.h"
#include <unordered_map>
#include <expected>

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

class ModelLoader;

class TextureManager {
	friend class ModelLoader;
public:
	bool load(const Texture& texture);
	bool unload(const Texture& texture);
	std::expected<unsigned int, bool> id(const std::string& path) const;

	TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator= (const TextureManager&) = delete;
	~TextureManager();

private:
	std::unordered_map<std::string, unsigned int> textures_loaded; // The textures loaded by OpenGL identified by their filepath mapped to their OpenGL id.

	bool load_file(const std::string& path);
	bool load_embedded(const std::string& path);
	bool load_data_compressed(const std::string& path, const void* data, unsigned int size);
	bool load_data(const std::string& path, const void* data, Format format, int width, int height);
};
