#include "Texture.h"

bool operator==(const TextureInfo& lhs, const TextureInfo& rhs) {
	return lhs.path == rhs.path;
}

Texture::Texture(unsigned int index, unsigned char generation) {
	id = 0;
	id |= (index & TEXTURE_INDEX_MASK);
	id |= (generation & TEXTURE_GENERATION_MASK) << TEXTURE_INDEX_BITS;
}

unsigned int Texture::index() const {
	return id & TEXTURE_INDEX_MASK;
}

unsigned char Texture::generation() const {
	return (id >> TEXTURE_INDEX_BITS) & TEXTURE_GENERATION_MASK;
}