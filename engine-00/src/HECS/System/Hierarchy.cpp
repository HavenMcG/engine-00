#include "Hierarchy.h"

void HierarchySystem::apply_parent_transforms() {
	for (int i = 0; i < relations.pc.size(); ++i) {
		Entity e = relations.pc.owners_[i];
		Entity parent = relations.pc.parents_[i];
		if (transforms.has_component(e) && transforms.has_component(parent)) {
			int it = transforms.indices_[e];
			int itp = transforms.indices_[parent];
			transforms.world_positions_[it] = transforms.world_positions_[itp] + transforms.positions_[it];
			transforms.world_rotations_[it] = transforms.world_rotations_[itp] * transforms.rotations_[it];
			transforms.world_scales_[it] = transforms.world_scales_[itp] * transforms.scales_[it];
		}
	}
}
