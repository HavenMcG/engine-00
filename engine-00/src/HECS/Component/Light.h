#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Entity/Entity.h"
#include "../../Error/Error.h"
#include <unordered_map>
#include <expected>

enum LightType {
	Point,
	Directional,
	Spotlight
};

struct Light {
	LightType type;
	glm::vec3 color;
	glm::vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float inner_cutoff;
	float outer_cuttoff;
};

struct LightRef {
	LightType& type;
	glm::vec3& color;
	glm::vec3& direction;
	float& constant;
	float& linear;
	float& quadratic;
	float& inner_cutoff;
	float& outer_cuttoff;
};

class LightCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<void, ErrorCode> add_component(Entity e);
	std::expected<void, ErrorCode> destroy_component(Entity e);
	std::expected<Light, ErrorCode> get_component(Entity e);
	std::expected<LightRef, ErrorCode> get_component_ref(Entity e);

	std::expected<void, ErrorCode> set_type(Entity e, LightType new_type);
	std::expected<void, ErrorCode> set_color(Entity e, glm::vec3 new_color);
	std::expected<void, ErrorCode> set_direction(Entity e, glm::vec3 new_direction);
	std::expected<void, ErrorCode> set_constant(Entity e, float new_constant);
	std::expected<void, ErrorCode> set_linear(Entity e, float new_linear);
	std::expected<void, ErrorCode> set_quadratic(Entity e, float new_quadratic);
	std::expected<void, ErrorCode> set_inner_cutoff(Entity e, float new_inner_cutoff);
	std::expected<void, ErrorCode> set_outer_cutoff(Entity e, float new_outer_cutoff);

	std::unordered_map<Entity, EntityType> indices_;
	std::vector<Entity> owners_;
	std::vector<LightType> types_;
	std::vector<glm::vec3> colors_;
	// for directional lights AND spotlights
	std::vector<glm::vec3> directions_;

	// for point lights:
	std::vector<float> constants_;
	std::vector<float> linears_;
	std::vector<float> quadratics_;

	// for spotlights
	std::vector<float> inner_cutoffs_;
	std::vector<float> outer_cutoffs_;

	EntityType n_ = 0;
};