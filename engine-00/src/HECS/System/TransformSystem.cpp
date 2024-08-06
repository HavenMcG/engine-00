#include "TransformSystem.h"

std::expected<void, ErrorCode> TransformSystem::rotate_degrees(Entity e, float pitch, float yaw, float roll) {
    auto r = transforms.indices_.find(e);
    if (r == transforms.indices_.end()) return std::unexpected(false);
    else transforms.world_rotations_[r->second] *= glm::quat(glm::vec3{glm::radians(pitch), glm::radians(yaw), glm::radians(roll)});
}

TransformSystem::TransformSystem(TransformCollection& transforms)
        : transforms{ transforms } {
}
