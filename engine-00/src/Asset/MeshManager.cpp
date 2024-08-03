#include "MeshManager.h"
#include "glad/glad.h"
#include <iostream>

void MeshManager::load_data(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
	bool already_loaded = (loaded_meshes.find(mesh.path) != loaded_meshes.end());
	if (!already_loaded) {
		std::cout << "loading mesh " << mesh.path << std::endl;
		unsigned int vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh.num_indices = indices.size();
		loaded_meshes.emplace(mesh.path, OGLMeshInfo{ vao, vbo, ebo });
	}
}

OGLMeshInfo MeshManager::info(const Mesh& mesh) {
	auto result = loaded_meshes.find(mesh.path);
	if (result != loaded_meshes.end()) return result->second;
	else return OGLMeshInfo{};
}
