#pragma once
#include "../Entity/Entity.h"
#include "../../Asset/Model.h"
#include <unordered_map>
#include <expected>

class ModelManager {
public:
	size_t size();
	const std::vector<Entity>& owners();
	const std::vector<Model>& models();
	std::expected<void, bool> add_component(Entity e, Model& model);
private:
	std::unordered_map<Entity, EntityType> map_; // map_[0] would contain the index of the actual component data for Entity 0

	std::vector<Entity> owners_; // we store the owning entities so we don't have to slow search the map_
	std::vector<Model> models_;
};