#pragma once
#include "../Data.h"
#include "../../HECS/Component/Model.h"
#include "../Storing/AssetStore.h"
#include <expected>
#include <string>

class AssetLoader {
public:
	virtual std::expected<Model, bool> load_model(const std::string& path, AssetStore& store) = 0;
	//virtual std::expected<Mesh, bool> load_mesh(const std::string& path, AssetStore& store) = 0;
	//virtual std::expected<Texture, bool> load_texture(const std::string& path, AssetStore& store) = 0;
};