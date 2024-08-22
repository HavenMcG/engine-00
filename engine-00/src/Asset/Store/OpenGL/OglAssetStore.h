#pragma once
#include "../AssetStore.h"
#include <deque>
#include <unordered_map>

class OglAssetStore : public AssetStore {
public:
	// meshes
	std::expected<Mesh, ErrorCode> load(const MeshData& data, const std::string& path) override;
	std::expected<Mesh, ErrorCode> load(const MeshData& data) override;
	std::expected<void, ErrorCode> unload(Mesh mesh) override;
	bool loaded(Mesh mesh) override;
	std::expected<Mesh, ErrorCode> mesh(const std::string& path) override;

	// textures
	std::expected<Texture, ErrorCode> load(const TextureData& data, const std::string& path) override;
	std::expected<Texture, ErrorCode> load(const TextureData& data) override;
	std::expected<void, ErrorCode> unload(Texture texture) override;
	bool loaded(Texture texture) override;
	std::expected<Texture, ErrorCode> texture(const std::string& path) override;

private:
	std::vector<unsigned char> mesh_generations_;
	std::deque<unsigned int> mesh_free_indices_;
public:
	std::vector<unsigned int> mesh_vaos_;
	std::vector<unsigned int> mesh_vbos_;
	std::vector<unsigned int> mesh_ebos_;
	std::vector<unsigned int> mesh_vertex_counts_;
	std::vector<unsigned int> mesh_index_counts_;
	std::vector<std::string> mesh_paths_;
	std::unordered_map<std::string, unsigned int> mesh_paths_to_indices_;

private:
	std::vector<unsigned char> tex_generations_;
	std::deque<unsigned int> tex_free_indices_;
public:
	std::vector<unsigned int> tex_ogl_ids_;
	std::vector<std::string> tex_paths_;
	std::unordered_map<std::string, unsigned int> tex_paths_to_indices_;
};