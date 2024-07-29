#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

enum Direction {
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

class Camera {
public:

	glm::mat4 view_matrix() const;
	glm::vec3 position() const;

	void move(Direction d, float amount);
	void move_to(glm::vec3 world_coord);
	void look_adjust(float delta_yaw, float delta_pitch);
	void look_set(float yaw, float pitch);
	void look_at(glm::vec3 world_coord);

	Camera(glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3 world_up = glm::vec3{ 0.0f, 1.0f, 0.0f }, float yaw = -90.0f, float pitch = 0.0f);
private:
	glm::vec3 position_;
	glm::vec3 front_;
	glm::vec3 up_;
	glm::vec3 right_;
	glm::vec3 world_up_;
	float yaw_;
	float pitch_;
	float zoom_;
	void update_vectors();
};