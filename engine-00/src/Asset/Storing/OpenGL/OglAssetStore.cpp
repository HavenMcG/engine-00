#include "OglAssetStore.h"
#include <glad/glad.h>
#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Meshes

std::expected<void, ErrorCode> OglAssetStore::load(const Mesh& mesh, const MeshData& data) {
	if (!loaded(mesh)) {
		std::cout << "loading mesh " << mesh.path << std::endl;
		if (mesh.num_indices != data.indices.size()) {
			std::cout << "ERROR loading mesh " << mesh.path << ": number of indices did not match data." << std::endl;
			return std::unexpected(false);
		}

		unsigned int vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

		loaded_meshes_.emplace(mesh.path, OGLMeshInfo{ vao, vbo, ebo });
	}
	else {
		std::cout << "skipping loading mesh " << mesh.path << " - already loaded" << std::endl;
		return std::unexpected(false);
	}
}

std::expected<void, ErrorCode> OglAssetStore::unload(const Mesh& mesh) {
	auto result = info(mesh);
	if (result.has_value()) {
		std::cout << "unloading mesh " << mesh.path << std::endl;
		OGLMeshInfo md = result.value();
		glDeleteVertexArrays(1, &md.vao);
		glDeleteBuffers(1, &md.vbo);
		glDeleteBuffers(1, &md.ebo);
		loaded_meshes_.erase(mesh.path);
	}
	else {
		std::cout << "could not unload mesh " << mesh.path << " - not loaded" << std::endl;
		return std::unexpected(result.error());
	}
}

bool OglAssetStore::loaded(const Mesh& mesh) {
    return loaded_meshes_.contains(mesh.path);
}

std::expected<OGLMeshInfo, bool> OglAssetStore::info(const Mesh& mesh) const {
	auto result = loaded_meshes_.find(mesh.path);
	if (result != loaded_meshes_.end()) return result->second;
	else return std::unexpected(false);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Textures

static GLenum to_gl_enum(Format f) {
	switch (f) {
		case RED: return GL_RED;
		case RGB: return GL_RGB;
		case RGBA: return GL_RGBA;
	}
}

std::expected<void, ErrorCode> OglAssetStore::load(const Texture& texture, const TextureData& data) {
	if (!loaded(texture)) {
		std::cout << "loading texture " << texture.path << std::endl;

		unsigned int tex_id;
		glGenTextures(1, &tex_id);

		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexImage2D(GL_TEXTURE_2D, 0, to_gl_enum(data.format), data.width, data.height, 0, to_gl_enum(data.format), GL_UNSIGNED_BYTE, data.data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		loaded_textures_.emplace(texture.path, OGLTextureInfo{ tex_id });
	}
	else {
		std::cout << "skipping loading texture " << texture.path << " - already loaded" << std::endl;
		return std::unexpected(false);
	}
}

std::expected<void, ErrorCode> OglAssetStore::unload(const Texture& texture) {
	auto result = info(texture);
	if (result.has_value()) {
		std::cout << "unloading texture " << texture.path << std::endl;
		OGLTextureInfo td = result.value();
		glDeleteTextures(1, &td.id);
		loaded_textures_.erase(texture.path);
	}
	else {
		std::cout << "could not unload texture " << texture.path << " - not loaded" << std::endl;
		return std::unexpected(result.error());
	}
}

bool OglAssetStore::loaded(const Texture& texture) {
	return loaded_textures_.contains(texture.path);
}

std::expected<OGLTextureInfo, ErrorCode> OglAssetStore::info(const Texture& texture) const {
	auto result = loaded_textures_.find(texture.path);
	if (result != loaded_textures_.end()) return result->second;
	else return std::unexpected(false);
}