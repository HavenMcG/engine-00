#include "Mesh.h"
#include "iostream"

Mesh::Mesh(const std::string& name) 
		: name{ name }
		, num_indices{ 0 } {
}