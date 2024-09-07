#pragma once
#include "../Entity/Entity.h"
#include "../../Error/Error.h"
#include <unordered_map>
#include <unordered_set>
#include <expected>
#include <string>

class ComponentCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;

	std::unordered_map<Entity, EntityType> indices_; // index of the component data for a given entity

	// The following containers need to have the same size.

	std::vector<Entity> owners_;
};