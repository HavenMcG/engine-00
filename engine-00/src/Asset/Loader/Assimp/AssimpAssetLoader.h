#pragma once
#include "../AssetLoader.h"

class AssimpAssetLoader : public AssetLoader {
public:
	std::expected<Model, bool> load_model(const std::string& path, AssetStore& store) override;
	//std::expected<Mesh, bool> load_mesh(const std::string& path, AssetStore& store) override;
	//std::expected<Texture, bool> load_texture(const std::string& path, AssetStore& store) override;

private:
	struct Private;
};