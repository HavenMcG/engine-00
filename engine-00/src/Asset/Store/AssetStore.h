#pragma once
#include "../Data.h"
#include "../Mesh.h"
#include "../Texture.h"
#include "../../Error/Error.h"
#include <expected>

class AssetStore {
public:
	// meshes
	
	// Path is only used to identify the mesh. AssetStore refuses to load the mesh if there is already one with the same path loaded. If you don't care about this you should use the single argument load() function which simply uses the mesh.id as the path.
	virtual std::expected<Mesh, ErrorCode> load(const MeshData& data, const std::string& path) = 0;
	virtual std::expected<Mesh, ErrorCode> load(const MeshData& data) = 0;
	virtual std::expected<void, ErrorCode> unload(Mesh mesh) = 0;
	virtual bool loaded(Mesh mesh) const = 0;
	// Returns the mesh with the given path, if it exists.
	virtual std::expected<Mesh, ErrorCode> mesh(const std::string& path) const = 0;
	virtual std::expected<Cuboid, ErrorCode> bounding_box(Mesh mesh) const = 0;

	// textures

	// Path is only used to identify the texture. AssetStore refuses to load the texture if there is already one with the same path loaded. If you don't care about this you should use the single argument load() function which simply uses the texture.id as the path.
	virtual std::expected<Texture, ErrorCode> load(const TextureData& data, const std::string& path) = 0;
	virtual std::expected<Texture, ErrorCode> load(const TextureData& data) = 0;
	virtual std::expected<void, ErrorCode> unload(Texture texture) = 0;
	virtual bool loaded(Texture texture) = 0;
	// Returns the texture with the given path, if it exists.
	virtual std::expected<Texture, ErrorCode> texture(const std::string& path) = 0;
};