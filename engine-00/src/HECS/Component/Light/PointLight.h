#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../../Entity/Entity.h"
#include "../../../Error/Error.h"
#include <unordered_map>
#include <expected>

struct PointLightRef;

struct PointLight {
	glm::vec3 color;
	float constant;
	float linear;
	float quadratic;
};

struct PointLightRef {
	glm::vec3& color;
	float& constant;
	float& linear;
	float& quadratic;
};


class PointLightCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<void, ErrorCode> add_component(Entity e);
	std::expected<void, ErrorCode> destroy_component(Entity e);
	std::expected<PointLight, ErrorCode> get_component(Entity e);
	std::expected<PointLightRef, ErrorCode> get_component_ref(Entity e);

	std::expected<void, ErrorCode> set_color(Entity e, glm::vec3 new_color);
	std::expected<void, ErrorCode> set_constant(Entity e, float new_constant);
	std::expected<void, ErrorCode> set_linear(Entity e, float new_linear);
	std::expected<void, ErrorCode> set_quadratic(Entity e, float new_quadratic);

	std::unordered_map<Entity, EntityType> indices_;
	std::vector<Entity> owners_;
	std::vector<glm::vec3> colors_;
	std::vector<float> constants_;
	std::vector<float> linears_;
	std::vector<float> quadratics_;


	EntityType n_ = 0;
};