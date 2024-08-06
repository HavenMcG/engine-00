#include "Model.h"
#include <iostream>

size_t ModelCollection::size() {
    return owners_.size();
}

const std::vector<Entity>& ModelCollection::owners() {
    return owners_;
}

const std::vector<Model>& ModelCollection::models() {
    return models_;
}

std::expected<void, bool> ModelCollection::add_component(Entity e, Model& model) {
    std::cout << "creating model component for entity " << e << std::endl;
    if (!map_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);
    models_.push_back(model);
}