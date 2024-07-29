#pragma once
#include "Mesh.h"
#include <unordered_map>

struct OGLMeshInfo {
	unsigned int vao, vbo, ebo;
};

class MeshManager {
public:
	void load_data(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	OGLMeshInfo info(const Mesh& mesh);
private:
	std::unordered_map<std::string, OGLMeshInfo> loaded_meshes; // The meshes loaded by OpenGL identified by their path mapped to OpenGL info struct
};