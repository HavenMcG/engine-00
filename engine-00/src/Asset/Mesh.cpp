#include "Mesh.h"

bool operator==(const Mesh& lhs, const Mesh& rhs) {
	return lhs.path == rhs.path;
}