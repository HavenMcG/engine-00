//#pragma once
//#include "../Mesh.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "glm/gtc/type_ptr.hpp"
//#include "../Texture.h"
//#include <expected>
//#include <vector>
//#include <unordered_map>
//
//struct Vertex {
//	glm::vec3 position;
//	glm::vec3 normal;
//	glm::vec2 tex_coords;
//};
//
//struct MeshData {
//	std::vector<Vertex> vertices;
//	std::vector<unsigned int> indices;
//};
//
//enum TextureType {
//	Diffuse,
//	Specular,
//	Ambient,
//	Emissive,
//	Heightmap,
//	Normalmap,
//	Shininess,
//	Opacity,
//	Displacement,
//	Lightmap,
//	Reflection,
//	UNIMPLEMENTED,
//	NONE
//};
//
//enum Format {
//	RED,
//	RGB,
//	RGBA
//};
//
//struct TextureData {
//	const void* data;
//	Format format;
//	int width;
//	int height;
//};
//
//class AssetUploader {
//public:
//	virtual std::expected<void, bool> upload_mesh_disk(const Mesh& mesh) = 0;
//	virtual std::expected<void, bool> upload_mesh_memory(Mesh& mesh, const MeshData& data) = 0;
//	virtual std::expected<void, bool> upload_texture_disk(const Texture& texture) = 0;
//	virtual std::expected<void, bool> upload_texture_memory(Texture& texture, const TextureData& data) = 0;
//};