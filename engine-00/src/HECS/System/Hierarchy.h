#pragma once
#include "../Component/Link.h"
#include "../Component/Transform.h"

class HierarchySystem {
public:
	LinkCollection& relations;
	TransformCollection& transforms;
	void apply_parent_transforms();
};