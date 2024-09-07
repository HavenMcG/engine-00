#pragma once
#include "../Component/Parent.h"
#include "../Component/Transform.h"

class HierarchySystem {
public:
	RelationCollection& relations;
	TransformCollection& transforms;
	void apply_parent_transforms();
};