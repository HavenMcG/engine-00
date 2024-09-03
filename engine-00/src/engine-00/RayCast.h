#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <optional>

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

glm::vec3 point_on_ray(Ray ray, float distance) {
	return ray.origin + distance * ray.direction;
}

std::optional<float> intersection_distance_xz(Ray ray) {
	if (ray.direction.y == 0.0f) return {};
	else return -ray.origin.y / ray.direction.y;
}

std::optional<glm::vec3> intersection_point_xz(Ray ray) {
	auto distance = intersection_distance_xz(ray);
	if (distance.has_value()) return point_on_ray(ray, *distance);
	else return {};
}