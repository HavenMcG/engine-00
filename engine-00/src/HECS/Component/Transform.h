#pragma once
#include "../Entity/Entity.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <unordered_map>
#include <expected>
#include "../../Error/Error.h"

struct TransformComponent {
	glm::vec3 world_position;
	glm::quat world_rotation;
};

class TransformCollection {
public:
	size_t size() const;
	const std::unordered_map<Entity, EntityType>& indices() const;
	const std::vector<Entity>& owners() const;
	const std::vector<glm::vec3>& world_positions() const;
	const std::vector<glm::quat>& world_rotations() const;

	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<TransformComponent, bool> component(Entity e) const;
	std::expected<glm::vec3, bool> position(Entity e) const;

	std::expected<void, bool> add_component(Entity e);
	std::expected<void, bool> add_component(Entity e, const TransformComponent& transform);
//private:
	std::unordered_map<Entity, EntityType> indices_; // index of the component data for a given entity

	// The following containers should always have the same size, otherwise the manager is invalid

	std::vector<Entity> owners_;
	std::vector<glm::vec3> world_positions_;
	std::vector<glm::quat> world_rotations_;
};