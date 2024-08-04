#pragma once
#include <string>

struct Texture {
	// The unique identifier of the texture. Used to compare textures - same path means they are considered equal.
	// If you are not loading from disk you can make the path whatever you'd like.
	std::string path;
};

bool operator==(const Texture& lhs, const Texture& rhs);

template<> struct std::hash<Texture> {
	std::size_t operator()(const Texture& texture) const {
		return std::hash<std::string>{}(texture.path);
	}
};