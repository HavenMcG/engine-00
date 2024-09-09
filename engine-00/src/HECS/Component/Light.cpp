#include "Light.h"

size_t LightCollection::size() const {
    return n_;
}

std::expected<EntityType, ErrorCode> LightCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool LightCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<void, ErrorCode> LightCollection::add_component(Entity e) {
    if (!indices_.emplace(e, n_).second) return std::unexpected(false);
    if (owners_.size() == n_) {
        owners_.push_back(e);
        ambients_.push_back(glm::vec3(0.0f));
        diffuses_.push_back(glm::vec3(0.0f));
        speculars_.push_back(glm::vec3(0.0f));
        constants_.push_back(0.0f);
        linears_.push_back(0.0f);
        quadratics_.push_back(0.0f);
        ++n_;
    }
    else {
        int i = n_ - 1;
        owners_[i] = e;
        ambients_[i] = glm::vec3(0.0f);
        diffuses_[i] = glm::vec3(0.0f);
        speculars_[i] = glm::vec3(0.0f);
        constants_[i] = 0.0f;
        linears_[i] = 0.0f;
        quadratics_[i] = 0.0f;
        ++n_;
    }
}

std::expected<void, ErrorCode> LightCollection::destroy_component(Entity e) {
    if (!has_component(e)) return std::unexpected{ false };
    int i = indices_[e];
    Entity last_entity = owners_[n_ - 1];
    int end = indices_[last_entity];

    owners_[i] = owners_[end];
    ambients_[i] = ambients_[end];
    diffuses_[i] = diffuses_[end];
    speculars_[i] = speculars_[end];
    constants_[i] = constants_[end];
    linears_[i] = linears_[end];
    quadratics_[i] = quadratics_[end];

    indices_[last_entity] = i;
    indices_.erase(e);
    --n_;
}

std::expected<Light, ErrorCode> LightCollection::get_component(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return Light{
        ambients_[i],
        diffuses_[i],
        speculars_[i],
        constants_[i],
        linears_[i],
        quadratics_[i]
    };
}

std::expected<LightRef, ErrorCode> LightCollection::get_component_ref(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return LightRef{
        ambients_[i],
        diffuses_[i],
        speculars_[i],
        constants_[i],
        linears_[i],
        quadratics_[i]
    };
}
