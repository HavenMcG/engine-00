#pragma once
#include <string>
#include <optional>
#include <expected>

class Texture {
	std::string name;
	std::optional<unsigned char*> pixel_data;
};