#include "PointLight.h"

size_t PointLightCollection::size() const {
    return n_;
}

std::expected<EntityType, ErrorCode> PointLightCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool PointLightCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<void, ErrorCode> PointLightCollection::add_component(Entity e) {
    if (!indices_.emplace(e, n_).second) return std::unexpected(false);
    if (owners_.size() == n_) {
        owners_.push_back(e);
        colors_.push_back(glm::vec3(0.0f));
        constants_.push_back(0.0f);
        linears_.push_back(0.0f);
        quadratics_.push_back(0.0f);
        ++n_;
    }
    else {
        int i = n_ - 1;
        owners_[i] = e;
        colors_[i] = glm::vec3(0.0f);
        constants_[i] = 0.0f;
        linears_[i] = 0.0f;
        quadratics_[i] = 0.0f;
        ++n_;
    }
}

std::expected<void, ErrorCode> PointLightCollection::destroy_component(Entity e) {
    if (!has_component(e)) return std::unexpected{ false };
    int i = indices_[e];
    Entity last_entity = owners_[n_ - 1];
    int end = indices_[last_entity];

    owners_[i] = owners_[end];
    colors_[i] = colors_[end];
    constants_[i] = constants_[end];
    linears_[i] = linears_[end];
    quadratics_[i] = quadratics_[end];

    indices_[last_entity] = i;
    indices_.erase(e);
    --n_;
}

std::expected<PointLight, ErrorCode> PointLightCollection::get_component(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return PointLight{
        colors_[i],
        constants_[i],
        linears_[i],
        quadratics_[i]
    };
}

std::expected<PointLightRef, ErrorCode> PointLightCollection::get_component_ref(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return PointLightRef{
        colors_[i],
        constants_[i],
        linears_[i],
        quadratics_[i]
    };
}

std::expected<void, ErrorCode> PointLightCollection::set_color(Entity e, glm::vec3 new_color) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    colors_[*r] = new_color;
}

std::expected<void, ErrorCode> PointLightCollection::set_constant(Entity e, float new_constant) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    constants_[*r] = new_constant;
}

std::expected<void, ErrorCode> PointLightCollection::set_linear(Entity e, float new_linear) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    linears_[*r] = new_linear;
}

std::expected<void, ErrorCode> PointLightCollection::set_quadratic(Entity e, float new_quadratic) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    quadratics_[*r] = new_quadratic;
}