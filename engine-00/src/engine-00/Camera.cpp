#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch)
		: position_{ position }
		, world_up_{ world_up }
		, yaw_{ yaw }
		, pitch_{ pitch }
		, zoom_{ 1.0f } {
	update_vectors();
}

void Camera::update_vectors() {
	front_ = glm::vec3{
		cos(glm::radians(yaw_)),
		sin(glm::radians(pitch_)),
		sin(glm::radians(yaw_))
	};
	front_ = glm::normalize(front_);
	right_ = glm::normalize(glm::cross(front_, world_up_));
	up_ = glm::normalize(glm::cross(right_, front_));
}

glm::mat4 Camera::view_matrix() const {
    return glm::lookAt(
		position_,
		position_ + front_, // target
		world_up_
	);
}

glm::vec3 Camera::position() const {
	return position_;
}

void Camera::move(Direction d, float amount) {
	if (d == Forward) {
		position_ += amount * front_;
	}
	else if (d == Backward) {
		position_ -= amount * front_;
	}
	else if (d == Left) {
		position_ -= right_ * amount;
	}
	else if (d == Right) {
		position_ += right_ * amount;
	}
	else if (d == Up) {
		position_ += amount * world_up_;
	}
	else if (d == Down) {
		position_ -= amount * world_up_;
	}
}

void Camera::move_to(glm::vec3 world_coord) {
	position_ = world_coord;
}

void Camera::look_adjust(float delta_yaw, float delta_pitch) {
	yaw_ += delta_yaw;
	pitch_ += delta_pitch;

	if (pitch_ > 89.99f) {
		pitch_ = 89.99f;
	}
	if (pitch_ < -89.99f) {
		pitch_ = -89.99f;
	}

	update_vectors();
}

void Camera::look_set(float yaw, float pitch) {
	yaw_ = yaw;
	pitch_ = pitch;
	if (pitch_ > 89.99f) {
		pitch_ = 89.99f;
	}
	if (pitch_ < -89.99f) {
		pitch_ = -89.99f;
	}
	update_vectors();
}

void Camera::look_at(glm::vec3 world_coord) {
	glm::vec3 d = glm::normalize(world_coord - position_);
	yaw_ = glm::degrees(atan2(d.z, d.x));
	pitch_ = glm::degrees(asin(d.y));
	update_vectors();
}
