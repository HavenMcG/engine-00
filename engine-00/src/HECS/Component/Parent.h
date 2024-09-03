#pragma once
#include "../Entity/Entity.h"
#include "../../Error/Error.h"
#include <unordered_map>
#include <expected>
#include <string>

struct ParentComponent {
	Entity parent;
	int depth;
};

class ParentCollection {
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<ParentComponent, bool> component(Entity e) const;

	std::expected<void, ErrorCode> add_component(Entity e, Entity parent);

	std::unordered_map<Entity, EntityType> indices_; // index of the component data for a given entity

	// The following containers need to have the same size.

	std::vector<Entity> owners_;
	std::vector<Entity> parents_;
	std::vector<int> depths_;
};