#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Entity/Entity.h"
#include "../../Error/Error.h"
#include <unordered_map>
#include <expected>

struct Light {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct LightRef {
	glm::vec3& ambient;
	glm::vec3& diffuse;
	glm::vec3& specular;

	float& constant;
	float& linear;
	float& quadratic;
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

	std::unordered_map<Entity, EntityType> indices_;
	std::vector<Entity> owners_;
	std::vector<glm::vec3> ambients_;
	std::vector<glm::vec3> diffuses_;
	std::vector<glm::vec3> speculars_;
	std::vector<float> constants_;
	std::vector<float> linears_;
	std::vector<float> quadratics_;


	EntityType n_ = 0;
};