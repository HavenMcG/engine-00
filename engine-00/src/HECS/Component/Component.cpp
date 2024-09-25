#include "Component.h"

size_t ComponentCollection::size() const {
    return n_;
}

std::expected<EntityType, ErrorCode> ComponentCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool ComponentCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<void, ErrorCode> ComponentCollection::add_component(Entity e) {
    if (!indices_.emplace(e, n_).second) return std::unexpected(false);
    if (owners_.size() == n_) {
        owners_.push_back(e);
        // [other fields here]
        ++n_;
    }
    else {
        int i = n_ - 1;
        owners_[i] = e;
        // [other fields here]
        ++n_;
    }
}

std::expected<void, ErrorCode> ComponentCollection::destroy_component(Entity e) {
    if (!has_component(e)) return std::unexpected{ false };
    int i = indices_[e];
    Entity last_entity = owners_[n_ - 1];
    int end = indices_[last_entity];
    owners_[i] = owners_[end];
    // [other fields here]
    indices_[last_entity] = i;
    indices_.erase(e);
    --n_;
}
