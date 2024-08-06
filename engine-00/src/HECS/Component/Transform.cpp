#include "Transform.h"

size_t TransformCollection::size() const {
    return owners_.size();
}

const std::unordered_map<Entity, EntityType>& TransformCollection::indices() const {
    return indices_;
}

const std::vector<Entity>& TransformCollection::owners() const {
    return owners_;
}

const std::vector<glm::vec3>& TransformCollection::world_positions() const {
    return world_positions_;
}

const std::vector<glm::quat>& TransformCollection::world_rotations() const {
    return world_rotations_;
}

std::expected<EntityType, ErrorCode> TransformCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool TransformCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<TransformComponent, bool> TransformCollection::component(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return TransformComponent{
        world_positions_[r->second],
        world_rotations_[r->second]
    };
}

std::expected<glm::vec3, bool> TransformCollection::position(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return world_positions_[r->second];
}

std::expected<void, bool> TransformCollection::add_component(Entity e) {
    if (!indices_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);
    world_positions_.push_back(glm::vec3{ 0,0,0 });
    world_rotations_.push_back(glm::quat{ 1,0,0,0 });
}

std::expected<void, bool> TransformCollection::add_component(Entity e, const TransformComponent& transform) {
    if (!indices_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);
    world_positions_.push_back(transform.world_position);
    world_rotations_.push_back(transform.world_rotation);
}