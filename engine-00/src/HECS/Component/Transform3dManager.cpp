#include "Transform3dManager.h"

size_t Transform3dManager::size() {
    return owners_.size();
}

const std::unordered_map<Entity, EntityType>& Transform3dManager::map() {
    return map_;
}

const std::vector<Entity>& Transform3dManager::owners() {
    return owners_;
}

const std::vector<glm::vec3>& Transform3dManager::positions() {
    return positions_;
}

const std::vector<glm::quat>& Transform3dManager::rotations() {
    return rotations_;
}

bool Transform3dManager::has_component(Entity e) {
    return map_.contains(e);
}

std::expected<void, bool> Transform3dManager::add_component(Entity e, glm::vec3 position) {
    if (!map_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);
    positions_.push_back(position);
    rotations_.push_back(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
}

std::expected<Transform3dComponentRef, bool> Transform3dManager::get_component(Entity e) {
    auto r = map_.find(e);
    if (r == map_.end()) return std::unexpected(false);
    else return Transform3dComponentRef{
        owners_[r->second],
        positions_[r->second],
        rotations_[r->second]
    };
}

std::expected<glm::vec3, bool> Transform3dManager::get_position(Entity e) {
    auto r = map_.find(e);
    if (r == map_.end()) return std::unexpected(false);
    return positions_[r->second];
}

std::expected<void, bool> Transform3dManager::rotate(Entity e, float pitch_degrees, float yaw_degrees, float roll_degrees) {
    auto r = map_.find(e);
    if (r == map_.end()) return std::unexpected(false);
    rotations_[r->second] *= glm::quat(glm::vec3{ glm::radians(pitch_degrees), glm::radians(yaw_degrees), glm::radians(roll_degrees) });
}