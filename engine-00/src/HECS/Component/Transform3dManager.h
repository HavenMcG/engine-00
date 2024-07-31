#pragma once
#include "../Entity/Entity.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <unordered_map>
#include <expected>

struct Transform3dComponentRef {
	const Entity owner;
	const glm::vec3& position;
	const glm::quat& rotation;
};

class Transform3dManager {
public:
	size_t size();
	const std::unordered_map<Entity, EntityType>& map();
	const std::vector<Entity>& owners();
	const std::vector<glm::vec3>& positions();
	const std::vector<glm::quat>& rotations();

	bool has_component(Entity e);
	std::expected<void, bool> add_component(Entity e, glm::vec3 position);
	std::expected<Transform3dComponentRef, bool> get_component(Entity e);
	std::expected<glm::vec3, bool> get_position(Entity e);
	std::expected<void, bool> rotate(Entity e, float pitch_degrees, float yaw_degrees, float roll_degrees);
private:
	std::unordered_map<Entity, EntityType> map_; // locations_[0] would contain the index of the actual component data for Entity 0 - this is the "sparse array".

	// The following containers should always have the same size, otherwise the manager is invalid

	std::vector<Entity> owners_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::quat> rotations_;
};