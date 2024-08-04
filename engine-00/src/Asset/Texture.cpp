#include "Texture.h"

bool operator==(const Texture& lhs, const Texture& rhs) {
	return lhs.path == rhs.path;
}
