#pragma once
#include <string>

const unsigned int TEXTURE_INDEX_BITS = 24;
const unsigned int TEXTURE_INDEX_MASK = (1 << TEXTURE_INDEX_BITS) - 1;

const unsigned int TEXTURE_GENERATION_BITS = 8;
const unsigned int TEXTURE_GENERATION_MASK = (1 << TEXTURE_INDEX_BITS) - 1;

struct Texture {
	unsigned int id;
	Texture(unsigned int index, unsigned char generation);
	unsigned int index() const;
	unsigned char generation() const;
};

struct TextureInfo {
	// The unique identifier of the texture. Used to compare textures - same path means they are considered equal.
	// If you are not loading from disk you can make the path whatever you'd like.
	std::string path;
};

bool operator==(const TextureInfo& lhs, const TextureInfo& rhs);

template<> struct std::hash<TextureInfo> {
	std::size_t operator()(const TextureInfo& texture) const {
		return std::hash<std::string>{}(texture.path);
	}
};