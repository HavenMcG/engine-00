#pragma once
#include "Texture.h"

struct Material {
	std::vector<Texture> diffuses;
	std::vector<Texture> speculars;
};