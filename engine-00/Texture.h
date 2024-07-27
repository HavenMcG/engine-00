#pragma once
#include <string>
#include <optional>
#include <expected>

struct Texture {
	std::string name;
	std::optional<unsigned char*> pixel_data;
	Texture(const std::string& name);
};