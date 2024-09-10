#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../../Entity/Entity.h"
#include "../../../Error/Error.h"
#include <unordered_map>
#include <expected>

struct DirectionalLight {
	glm::vec3 color;
	glm::vec3 direction;
};

struct DirectionalLightRef {
	glm::vec3& color;
	glm::vec3& direction;
};

class DirectionalLightCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<void, ErrorCode> add_component(Entity e);
	std::expected<void, ErrorCode> destroy_component(Entity e);
	std::expected<DirectionalLight, ErrorCode> get_component(Entity e);
	std::expected<DirectionalLightRef, ErrorCode> get_component_ref(Entity e);

	std::expected<void, ErrorCode> set_color(Entity e, glm::vec3 new_color);
	std::expected<void, ErrorCode> set_direction(Entity e, glm::vec3 new_direction);

	std::unordered_map<Entity, EntityType> indices_;
	std::vector<Entity> owners_;
	std::vector<glm::vec3> colors_;
	std::vector<glm::vec3> directions_;


	EntityType n_ = 0;
};