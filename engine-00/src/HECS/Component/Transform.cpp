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

const std::vector<glm::vec3>& TransformCollection::scales() const {
    return scales_;
}

std::expected<EntityType, ErrorCode> TransformCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool TransformCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<Transform, bool> TransformCollection::transform(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return Transform{
        positions_[r->second],
        rotations_[r->second],
        scales_[r->second],
        position_offsets_[r->second]
    };
}

std::expected<Transform, bool> TransformCollection::world_transform(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return Transform{
        world_positions_[r->second],
        world_rotations_[r->second],
        world_scales_[r->second],
        position_offsets_[r->second]
    };
}

std::expected<glm::vec3, bool> TransformCollection::position(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return positions_[r->second];
}

std::expected<void, ErrorCode> TransformCollection::set_position(Entity e, glm::vec3 new_position) {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    glm::vec3 delta = new_position - positions_[r->second];
    positions_[r->second] = new_position;
    world_positions_[r->second] += delta;
}

std::expected<glm::quat, ErrorCode> TransformCollection::rotation(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return rotations_[r->second];
}

std::expected<void, ErrorCode> TransformCollection::set_rotation(Entity e, glm::quat new_rotation) {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    glm::quat delta = glm::inverse(new_rotation) * rotations_[r->second];
    rotations_[r->second] = new_rotation;
    world_rotations_[r->second] *= delta;
}

std::expected<glm::vec3, ErrorCode> TransformCollection::scale(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return scales_[r->second];
}

std::expected<void, ErrorCode> TransformCollection::set_scale(Entity e, glm::vec3 new_scale) {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    glm::vec3 delta = new_scale / scales_[r->second];
    scales_[r->second] = new_scale;
    world_scales_[r->second] *= delta;
}

std::expected<glm::vec3, ErrorCode> TransformCollection::position_offset(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return position_offsets_[r->second];
}

std::expected<void, ErrorCode> TransformCollection::set_position_offset(Entity e, glm::vec3 new_offset) {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    position_offsets_[r->second] = new_offset;
}

std::expected<glm::vec3, bool> TransformCollection::world_position(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return world_positions_[r->second];
}

std::expected<glm::quat, ErrorCode> TransformCollection::world_rotation(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return world_rotations_[r->second];
}

std::expected<glm::vec3, ErrorCode> TransformCollection::world_scale(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    return world_scales_[r->second];
}

std::expected<void, bool> TransformCollection::add_component(Entity e) {
    if (!indices_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);

    positions_.push_back(glm::vec3{ 0,0,0 });
    rotations_.push_back(glm::quat{ 1,0,0,0 });
    scales_.push_back(glm::vec3{ 1,1,1 });
    position_offsets_.push_back(glm::vec3{ 0,0,0 });

    world_positions_.push_back(glm::vec3{ 0,0,0 });
    world_rotations_.push_back(glm::quat{ 1,0,0,0 });
    world_scales_.push_back(glm::vec3{ 1,1,1 });
}

std::expected<void, bool> TransformCollection::add_component(Entity e, const Transform& transform) {
    if (!indices_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);

    positions_.push_back(transform.position);
    rotations_.push_back(transform.rotation);
    scales_.push_back(transform.scale);
    position_offsets_.push_back(transform.position_offset);

    world_positions_.push_back(transform.position);
    world_rotations_.push_back(transform.rotation);
    world_scales_.push_back(transform.scale);
}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 position_offset)
    : position{ position }
    , rotation{ rotation }
    , scale{ scale }
    , position_offset{ position_offset }
{
}