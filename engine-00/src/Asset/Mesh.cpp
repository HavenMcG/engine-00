#include "Mesh.h"
#include "iostream"

Mesh::Mesh(const std::string& name) 
		: path{ name }
		, num_indices{ 0 } {
}