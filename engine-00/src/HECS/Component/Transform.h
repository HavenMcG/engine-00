#pragma once
#include "../Entity/Entity.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <unordered_map>
#include <expected>
#include "../../Error/Error.h"

struct Transform {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 position_offset;
	Transform(glm::vec3 position = { 0,0,0 }, glm::quat rotation = { 1,0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::vec3 position_offset = { 0,0,0 });
};

class TransformCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<Transform, bool> transform(Entity e) const;
	std::expected<Transform, bool> world_transform(Entity e) const;

	std::expected<void, bool> add_component(Entity e);
	std::expected<void, bool> add_component(Entity e, const Transform& transform);

	// mass getters:
	const std::unordered_map<Entity, EntityType>& indices() const;
	const std::vector<Entity>& owners() const;
	const std::vector<glm::vec3>& world_positions() const;
	const std::vector<glm::quat>& world_rotations() const;
	const std::vector<glm::vec3>& scales() const;

	// individual field getters/setters:
	std::expected<glm::vec3, ErrorCode> position(Entity e) const;
	std::expected<void, ErrorCode> set_position(Entity e, glm::vec3 new_position);
	std::expected<glm::quat, ErrorCode> rotation(Entity e) const;
	std::expected<void, ErrorCode> set_rotation(Entity e, glm::quat new_rotation);
	std::expected<glm::vec3, ErrorCode> scale(Entity e) const;
	std::expected<void, ErrorCode> set_scale(Entity e, glm::vec3 new_scale);

	std::expected<glm::vec3, ErrorCode> position_offset(Entity e) const;
	std::expected<void, ErrorCode> set_position_offset(Entity e, glm::vec3 new_offset);

	std::expected<glm::vec3, ErrorCode> world_position(Entity e) const;
	std::expected<glm::quat, ErrorCode> world_rotation(Entity e) const;
	std::expected<glm::vec3, ErrorCode> world_scale(Entity e) const;

//private:
	std::unordered_map<Entity, EntityType> indices_; // index of the component data for a given entity

	// The following containers need to have the same size.

	std::vector<Entity> owners_;

	std::vector<glm::vec3> positions_;
	std::vector<glm::quat> rotations_;
	std::vector<glm::vec3> scales_;
	std::vector<glm::vec3> position_offsets_;

	std::vector<glm::vec3> world_positions_;
	std::vector<glm::quat> world_rotations_;
	std::vector<glm::vec3> world_scales_;
};