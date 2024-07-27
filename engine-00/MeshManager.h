#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Mesh.h"

struct OGLMeshInfo {
	unsigned int vao, vbo, ebo;
};

class MeshManager {
public:
	void ogl_load_mesh(Mesh* target);
	OGLMeshInfo get_ogl_info(Mesh* mesh);
private:
	std::unordered_map<std::string, size_t> loaded_meshes; // the meshes loaded by OpenGL identified by their path mapped to their index in the container.
	std::vector<OGLMeshInfo> meshes;
};