#include "Parent.h"

size_t ParentCollection::size() const {
    return owners_.size();
}

std::expected<EntityType, ErrorCode> ParentCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool ParentCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<ParentComponent, bool> ParentCollection::component(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return ParentComponent{
        parents_[r->second],
        depths_[r->second]
    };
}

std::expected<void, ErrorCode> ParentCollection::add_component(Entity e, Entity parent) {
    if (!indices_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);
    parents_.push_back(parent);
    if (has_component(parent)) {
        depths_.push_back(depths_[*index(parent)] + 1);
    }
    else {
        depths_.push_back(0);
    }
    for (int i = 0; i < parents_.size(); ++i) {
        if (parents_[i] == e) {
            ++depths_[i];
        }
    }
}
