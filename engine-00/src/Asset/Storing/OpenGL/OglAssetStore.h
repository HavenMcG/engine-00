#pragma once
#include "../AssetStore.h"
#include <unordered_map>

struct OGLMeshInfo {
	unsigned int vao, vbo, ebo;
};

struct OGLTextureInfo {
	unsigned int id;
};

class OglAssetStore : public AssetStore {
public:
	// meshes
	std::expected<void, ErrorCode> load(const Mesh& mesh, const MeshData& data) override;
	std::expected<void, ErrorCode> unload(const Mesh& mesh) override;
	bool loaded(const Mesh& mesh) override;
	std::expected<Mesh, ErrorCode> key(const Mesh& mesh) override;
	std::expected<OGLMeshInfo, ErrorCode> info(const Mesh& mesh) const;

	// textures
	std::expected<void, ErrorCode> load(const Texture& texture, const TextureData& data) override;
	std::expected<void, ErrorCode> unload(const Texture& texture) override;
	bool loaded(const Texture& texture) override;
	std::expected<Texture, ErrorCode> key(const Texture& texture) override;
	std::expected<OGLTextureInfo, ErrorCode> info(const Texture& texture) const;

private:
	std::unordered_map<Mesh, OGLMeshInfo> loaded_meshes_;
	std::unordered_map<Texture, OGLTextureInfo> loaded_textures_;
};