#include "OglAssetStore.h"
#include <glad/glad.h>
#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Meshes

std::expected<Mesh, ErrorCode> OglAssetStore::load(const MeshData& data, const std::string& name) {
	std::cout << "loading mesh " << name << std::endl;
	if (!mesh_paths_to_indices_.contains(name)) {
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
			mesh_paths_.push_back(name);
			mesh_bounding_boxes_.push_back(generate_bounding_box(data));
			mesh_paths_to_indices_.emplace(name, index);
		}
		else {
			index = mesh_free_indices_.front();
			mesh_free_indices_.pop_front();
			mesh_vaos_[index] = vao;
			mesh_vbos_[index] = vbo;
			mesh_ebos_[index] = ebo;
			mesh_vertex_counts_[index] = data.vertices.size();
			mesh_index_counts_[index] = data.indices.size();
			mesh_paths_[index] = name;
			mesh_bounding_boxes_[index] = generate_bounding_box(data);
			mesh_paths_to_indices_.emplace(name, index);
		}
		return Mesh{ index, mesh_generations_[index] };
	}
	else {
		std::cout << "error loading mesh " << name << " - a mesh with that path is already loaded";
		return std::unexpected(false);
	}
}

std::expected<Mesh, ErrorCode> OglAssetStore::load(const MeshData& data) {
	unsigned int index;
	unsigned char generation;
	if (mesh_free_indices_.empty()) {
		index = mesh_generations_.size();
		generation = 0;
	}
	else {
		index = mesh_free_indices_.front();
		generation = mesh_generations_[index];
	}
	return load(data, std::to_string(Mesh::calc_id(index, generation)));
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
	mesh_paths_to_indices_.erase(mesh_paths_[i]);
}

bool OglAssetStore::loaded(Mesh mesh) const {
	if (mesh.index() >= mesh_generations_.size()) return false;
	return mesh_generations_[mesh.index()] == mesh.generation();
}

std::expected<Mesh, ErrorCode> OglAssetStore::mesh(const std::string& path) const {
	auto r = mesh_paths_to_indices_.find(path);
	if (r == mesh_paths_to_indices_.end()) return std::unexpected(false);
	else return Mesh{ r->second, mesh_generations_[r->second] };
}

std::expected<Cuboid, ErrorCode> OglAssetStore::bounding_box(Mesh mesh) const {
	if (!loaded(mesh)) return std::unexpected(false);
	return mesh_bounding_boxes_[mesh.index()];
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

std::expected<Texture, ErrorCode> OglAssetStore::load(const TextureData& data, const std::string& name) {
	std::cout << "loading texture " << name;
	if (data.format == RGBA) std::cout << " RGBA";
	std::cout << std::endl;
	if (!tex_paths_to_indices_.contains(name)) {
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
			tex_paths_.push_back(name);
			tex_paths_to_indices_.emplace(name, index);
		}
		else {
			index = tex_free_indices_.front();
			tex_free_indices_.pop_front();
			tex_ogl_ids_[index] = ogl_id;
			tex_paths_[index] = name;
			tex_paths_to_indices_.emplace(name, index);
		}
		return Texture{ index, tex_generations_[index] };
	}
	else {
		std::cout << "error loading texture " << name << " - a texture with that path is already loaded";
		return std::unexpected(false);
	}
}

std::expected<Texture, ErrorCode> OglAssetStore::load(const TextureData& data) {
	unsigned int index;
	unsigned char generation;
	if (tex_free_indices_.empty()) {
		index = tex_generations_.size();
		generation = 0;
	}
	else {
		index = tex_free_indices_.front();
		generation = tex_generations_[index];
	}
	return load(data, std::to_string(Texture::calc_id(index, generation)));
}

std::expected<void, ErrorCode> OglAssetStore::unload(Texture texture) {
	if (!loaded(texture)) return std::unexpected(false);
	unsigned int i = texture.index();
	glDeleteTextures(1, &tex_ogl_ids_[i]);
	tex_free_indices_.push_back(i);
	++tex_generations_[i];
	tex_paths_to_indices_.erase(tex_paths_[i]);
}

bool OglAssetStore::loaded(Texture texture) {
	if (texture.index() >= tex_generations_.size()) return false;
	return tex_generations_[texture.index()] == texture.generation();
}

std::expected<Texture, ErrorCode> OglAssetStore::texture(const std::string& path) {
	auto r = tex_paths_to_indices_.find(path);
	if (r == tex_paths_to_indices_.end()) return std::unexpected(false);
	else return Texture{ r->second, tex_generations_[r->second] };
}
