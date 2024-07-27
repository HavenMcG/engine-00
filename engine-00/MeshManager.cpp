#include "MeshManager.h"
#include "glad/glad.h"
#include <iostream>

void MeshManager::ogl_load_mesh(Mesh* target) {
	bool already_loaded = (loaded_meshes.find(target->name) != loaded_meshes.end());
	if (!already_loaded) {
		std::cout << "loading mesh \"" << target->name << "\"" << std::endl;
		unsigned int vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, target->vertices.size() * sizeof(Vertex), target->vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, target->indices.size() * sizeof(unsigned int), target->indices.data(), GL_STATIC_DRAW);

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
		meshes.push_back(OGLMeshInfo{ vao,vbo,ebo });
		loaded_meshes.emplace(target->name, meshes.size() - 1);
	}
}

OGLMeshInfo MeshManager::get_ogl_info(Mesh* mesh) {
	auto result = loaded_meshes.find(mesh->name);
	if (result != loaded_meshes.end()) return meshes[result->second];
	else return OGLMeshInfo{};
}
