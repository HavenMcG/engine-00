#pragma once
#include "../Data.h"
#include "../Mesh.h"
#include "../Texture.h"
#include "../../Error/Error.h"
#include <expected>

class AssetStore {
public:
	// meshes
	virtual std::expected<void, ErrorCode> load(const Mesh& mesh, const MeshData& data) = 0;
	virtual std::expected<void, ErrorCode> unload(const Mesh& mesh) = 0;
	virtual bool loaded(const Mesh& mesh) = 0;
	virtual std::expected<Mesh, ErrorCode> key(const Mesh& mesh) = 0;

	// textures
	virtual std::expected<void, ErrorCode> load(const Texture& texture, const TextureData& data) = 0;
	virtual std::expected<void, ErrorCode> unload(const Texture& texture) = 0;
	virtual bool loaded(const Texture& texture) = 0;
	virtual std::expected<Texture, ErrorCode> key(const Texture& texture) = 0;
};