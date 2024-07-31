#include "ModelManager.h"
#include <iostream>

size_t ModelManager::size() {
    return owners_.size();
}

const std::vector<Entity>& ModelManager::owners() {
    return owners_;
}

const std::vector<Model>& ModelManager::models() {
    return models_;
}

std::expected<void, bool> ModelManager::add_component(Entity e, Model& model) {
    std::cout << "creating model component for entity " << e << std::endl;
    if (!map_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);
    models_.push_back(model);
}