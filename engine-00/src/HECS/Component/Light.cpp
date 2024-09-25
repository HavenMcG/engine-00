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
        types_.push_back(Point);
        colors_.push_back({ 0.0f, 0.0f, 0.0f });
        directions_.push_back({ 0.0f, 0.0f, 0.0f });
        constants_.push_back(0.0f);
        linears_.push_back(0.0f);
        quadratics_.push_back(0.0f);
        inner_cutoffs_.push_back(0.0f);
        outer_cutoffs_.push_back(0.0f);
        ++n_;
    }
    else {
        int i = n_ - 1;
        owners_[i] = e;
        types_[i] = Point;
        colors_[i] = { 0.0f, 0.0f, 0.0f };
        directions_[i] = { 0.0f, 0.0f, 0.0f };
        constants_[i] = 0.0f;
        linears_[i] = 0.0f;
        quadratics_[i] = 0.0f;
        inner_cutoffs_[i] = 0.0f;
        outer_cutoffs_[i] = 0.0f;
        ++n_;
    }
}

std::expected<void, ErrorCode> LightCollection::destroy_component(Entity e) {
    if (!has_component(e)) return std::unexpected{ false };
    int i = indices_[e];
    Entity last_entity = owners_[n_ - 1];
    int end = indices_[last_entity];

    owners_[i] = owners_[end];
    types_[i] = types_[end];
    colors_[i] = colors_[end];
    directions_[i] = directions_[end];
    constants_[i] = constants_[end];
    linears_[i] = linears_[end];
    quadratics_[i] = quadratics_[end];
    inner_cutoffs_[i] = inner_cutoffs_[end];
    outer_cutoffs_[i] = outer_cutoffs_[end];

    indices_[last_entity] = i;
    indices_.erase(e);
    --n_;
}

std::expected<Light, ErrorCode> LightCollection::get_component(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return Light{
        types_[i],
        colors_[i],
        directions_[i],
        constants_[i],
        linears_[i],
        quadratics_[i],
        inner_cutoffs_[i],
        outer_cutoffs_[i]
    };
}

std::expected<LightRef, ErrorCode> LightCollection::get_component_ref(Entity e) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    int i = *r;
    return LightRef{
        types_[i],
        colors_[i],
        directions_[i],
        constants_[i],
        linears_[i],
        quadratics_[i],
        inner_cutoffs_[i],
        outer_cutoffs_[i]
    };
}

std::expected<void, ErrorCode> LightCollection::set_type(Entity e, LightType new_type) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    types_[*r] = new_type;
}

std::expected<void, ErrorCode> LightCollection::set_color(Entity e, glm::vec3 new_color) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    colors_[*r] = new_color;
}

std::expected<void, ErrorCode> LightCollection::set_direction(Entity e, glm::vec3 new_direction) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    directions_[*r] = new_direction;
}

std::expected<void, ErrorCode> LightCollection::set_constant(Entity e, float new_constant) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    constants_[*r] = new_constant;
}

std::expected<void, ErrorCode> LightCollection::set_linear(Entity e, float new_linear) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    linears_[*r] = new_linear;
}

std::expected<void, ErrorCode> LightCollection::set_quadratic(Entity e, float new_quadratic) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    quadratics_[*r] = new_quadratic;
}

std::expected<void, ErrorCode> LightCollection::set_inner_cutoff(Entity e, float new_inner_cutoff) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    inner_cutoffs_[*r] = new_inner_cutoff;
}

std::expected<void, ErrorCode> LightCollection::set_outer_cutoff(Entity e, float new_outer_cutoff) {
    auto r = index(e);
    if (!r.has_value()) return std::unexpected(false);
    outer_cutoffs_[*r] = new_outer_cutoff;
}
