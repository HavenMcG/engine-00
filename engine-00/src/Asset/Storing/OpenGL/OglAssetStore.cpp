#include "OglAssetStore.h"
#include <glad/glad.h>
#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Meshes

std::expected<Mesh, ErrorCode> OglAssetStore::load(const MeshData& data) {
	std::cout << "loading mesh" << std::endl;

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

	unsigned int index;
	if (mesh_free_indices_.empty()) {
		index = mesh_generations_.size();
		mesh_generations_.push_back(0);
		mesh_vaos_.push_back(vao);
		mesh_vbos_.push_back(vbo);
		mesh_ebos_.push_back(ebo);
		mesh_vertex_counts_.push_back(data.vertices.size());
		mesh_index_counts_.push_back(data.indices.size());
	}
	else {
		index = mesh_free_indices_.front();
		mesh_free_indices_.pop_front();
		mesh_vaos_[index] = vao;
		mesh_vbos_[index] = vbo;
		mesh_ebos_[index] = ebo;
		mesh_vertex_counts_[index] = data.vertices.size();
		mesh_index_counts_[index] = data.indices.size();
	}
	return Mesh{ index, mesh_generations_[index] };
}

std::expected<void, ErrorCode> OglAssetStore::unload(Mesh mesh) {
	if (!loaded(mesh)) return std::unexpected(false);
	std::cout << "unloading mesh" << std::endl;
	unsigned int i = mesh.index();
	glDeleteVertexArrays(1, &mesh_vaos_[i]);
	glDeleteBuffers(1, &mesh_vbos_[i]);
	glDeleteBuffers(1, &mesh_ebos_[i]);
	mesh_free_indices_.push_back(i);
	++mesh_generations_[i];
}

bool OglAssetStore::loaded(Mesh mesh) {
	if (mesh.index() >= tex_generations_.size()) return false;
	return tex_generations_[mesh.index()] == mesh.generation();
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

std::expected<Texture, ErrorCode> OglAssetStore::load(const TextureData& data) {
	std::cout << "loading texture" << std::endl;

	unsigned int ogl_id;
	glGenTextures(1, &ogl_id);

	glBindTexture(GL_TEXTURE_2D, ogl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, to_gl_enum(data.format), data.width, data.height, 0, to_gl_enum(data.format), GL_UNSIGNED_BYTE, data.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int index;
	if (tex_free_indices_.empty()) {
		index = tex_ogl_ids_.size();
		tex_generations_.push_back(0);
		tex_ogl_ids_.push_back(ogl_id);
	}
	else {
		index = tex_free_indices_.front();
		tex_free_indices_.pop_front();
		tex_ogl_ids_[index] = ogl_id;
	}
	return Texture{ index, tex_generations_[index] };
}

std::expected<void, ErrorCode> OglAssetStore::unload(Texture texture) {
	if (!loaded(texture)) return std::unexpected(false);
	unsigned int i = texture.index();
	glDeleteTextures(1, &tex_ogl_ids_[i]);
	tex_free_indices_.push_back(i);
	++tex_generations_[i];
}

bool OglAssetStore::loaded(Texture texture) {
	if (texture.index() >= tex_generations_.size()) return false;
	return tex_generations_[texture.index()] == texture.generation();
}