#include "SpotLight.h"

size_t SpotLightCollection::size() const {
    return n_;
}

std::expected<EntityType, ErrorCode> SpotLightCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool SpotLightCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<void, ErrorCode> SpotLightCollection::add_component(Entity e) {
    if (!indices_.emplace(e, n_).second) return std::unexpected(false);
    if (owners_.size() == n_) {
        owners_.push_back(e);
        colors_.push_back(glm::vec3(0.0f));
        directions_.push_back(glm::vec3(0.0f));
        ++n_;
    }
    else {
        int i = n_ - 1;
        owners_[i] = e;
        colors_[i] = glm::vec3(0.0f);
        directions_[i] = glm::vec3(0.0f);
        ++n_;
    }
}

std::expected<void, ErrorCode> SpotLightCollection::destroy_component(Entity e) {
    if (!has_component(e)) return std::unexpected{ false };
    int i = indices_[e];
    Entity last_entity = owners_[n_ - 1];
    int end = indices_[last_entity];

    owners_[i] = owners_[end];
    colors_[i] = colors_[end];
    directions_[i] = directions_[end];

    indices_[last_entity] = i;
    indices_.erase(e);
    --n_;
}

std::expected<SpotLight, ErrorCode> SpotLightCollection::get_component(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return SpotLight{
        colors_[i],
        directions_[i]
    };
}

std::expected<SpotLightRef, ErrorCode> SpotLightCollection::get_component_ref(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return SpotLightRef{
        colors_[i],
        directions_[i]
    };
}

std::expected<void, ErrorCode> SpotLightCollection::set_color(Entity e, glm::vec3 new_color) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    colors_[*r] = new_color;
}

std::expected<void, ErrorCode> SpotLightCollection::set_direction(Entity e, glm::vec3 new_direction) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    directions_[*r] = new_direction;
}
