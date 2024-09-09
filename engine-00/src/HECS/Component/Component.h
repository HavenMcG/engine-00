#pragma once
#include "../Entity/Entity.h"
#include "../../Error/Error.h"
#include <unordered_map>
#include <expected>

class ComponentCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<void, ErrorCode> add_component(Entity e);
	std::expected<void, ErrorCode> destroy_component(Entity e);

	std::unordered_map<Entity, EntityType> indices_;
	std::vector<Entity> owners_;
	// [other fields here]

	EntityType n_ = 0;
};