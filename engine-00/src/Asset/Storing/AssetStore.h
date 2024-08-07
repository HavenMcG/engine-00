#pragma once
#include "../Data.h"
#include "../Mesh.h"
#include "../Texture.h"
#include "../../Error/Error.h"
#include <expected>

class AssetStore {
public:
	// meshes
	virtual std::expected<Mesh, ErrorCode> load(const MeshData& data) = 0;
	virtual std::expected<void, ErrorCode> unload(Mesh mesh) = 0;
	virtual bool loaded(Mesh mesh) = 0;

	// textures
	virtual std::expected<Texture, ErrorCode> load(const TextureData& data) = 0;
	virtual std::expected<void, ErrorCode> unload(Texture texture) = 0;
	virtual bool loaded(Texture texture) = 0;
};