#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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

class TextureManager {
public:
	unsigned int load_texture_file(const std::string& path);
	unsigned int load_embedded_texture(const aiTexture* ai_tex, const std::string& path);
	unsigned int load_texture_data(const void* data, unsigned int format, int width, int height);
	bool find_id(const std::string& path, unsigned int& id);

	std::unordered_map<std::string, unsigned int> textures_loaded; // the textures loaded by OpenGL identified by their filepath mapped to their OpenGL id.

	TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator= (const TextureManager&) = delete;
	~TextureManager() = default;
};

TextureType convert_texture_type(aiTextureType ai_ttype);