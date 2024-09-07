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
    //std::cout << "creating model component for entity " << e << std::endl;
    if (!map_.emplace(e, owners_.size()).second) return std::unexpected(false);
    owners_.push_back(e);
    models_.push_back(model);
}

std::expected<Cuboid, ErrorCode> ModelCollection::bounding_box(Entity e, const AssetStore& as) {
    auto r = map_.find(e);
    if (r == map_.end()) return std::unexpected(false);
    const auto& meshes = models_[r->second].meshes;
    Cuboid bounds = *as.bounding_box(meshes[0]);
    Cuboid c;
    for (int i = 1; i < meshes.size(); ++i) {
        c = *as.bounding_box(meshes[i]);
        glm::vec3 min_corner;
        glm::vec3 max_corner;

        // Calculate the minimum corner of the enclosing cuboid
        bounds.p1.x = std::min({ c.p1.x, c.p2.x, bounds.p1.x, bounds.p2.x });
        bounds.p1.y = std::min({ c.p1.y, c.p2.y, bounds.p1.y, bounds.p2.y });
        bounds.p1.z = std::min({ c.p1.z, c.p2.z, bounds.p1.z, bounds.p2.z });

        // Calculate the maximum corner of the enclosing cuboid
        bounds.p2.x = std::max({ c.p1.x, c.p2.x, bounds.p1.x, bounds.p2.x });
        bounds.p2.y = std::max({ c.p1.y, c.p2.y, bounds.p1.y, bounds.p2.y });
        bounds.p2.z = std::max({ c.p1.z, c.p2.z, bounds.p1.z, bounds.p2.z });
    }
    return bounds;
}
