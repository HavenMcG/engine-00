#include "Component.h"

size_t ComponentCollection::size() const {
    return owners_.size();
}

std::expected<EntityType, ErrorCode> ComponentCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool ComponentCollection::has_component(Entity e) const {
    return indices_.contains(e);
}
