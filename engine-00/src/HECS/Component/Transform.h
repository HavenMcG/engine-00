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
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<TransformComponent, bool> component(Entity e) const;

	std::expected<void, bool> add_component(Entity e);
	std::expected<void, bool> add_component(Entity e, const TransformComponent& transform);

	// mass getters:
	const std::unordered_map<Entity, EntityType>& indices() const;
	const std::vector<Entity>& owners() const;
	const std::vector<glm::vec3>& world_positions() const;
	const std::vector<glm::quat>& world_rotations() const;

	// individual field getters/setters:
	std::expected<glm::vec3, bool> world_position(Entity e) const;
	std::expected<void, ErrorCode> set_world_position(Entity e, glm::vec3 new_position);
	std::expected<glm::quat, ErrorCode> world_rotation(Entity e) const;
	std::expected<void, ErrorCode> set_world_rotation(Entity e, glm::quat new_rotation);

//private:
	std::unordered_map<Entity, EntityType> indices_; // index of the component data for a given entity

	// The following containers need to have the same size.

	std::vector<Entity> owners_;
	std::vector<glm::vec3> world_positions_;
	std::vector<glm::quat> world_rotations_;
};