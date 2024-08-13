#include "HexGrid.h"

float Orientation::angle() const {
	return angle_;
}

glm::mat3x2 Orientation::axes() const {
	return axes_;
}

glm::mat2 Orientation::forward_matrix() const {
	return forward_;
}

glm::mat2 Orientation::inverse_matrix() const {
	return inverse_;
}

Orientation::Orientation(float angle)
		: angle_{ angle } {
	calc_axes();
	calc_forward();
	inverse_ = glm::inverse(forward_);
}

void Orientation::calc_axes() {
	auto theta = glm::radians(angle_ * 60.0f);
	glm::mat2 rotation_matrix{
		cos(theta), -sin(theta),
		sin(theta), cos(theta)
	};
	axes_ = glm::mat3x2{
		AXES[0] * rotation_matrix,
		AXES[1] * rotation_matrix,
		AXES[2] * rotation_matrix
	};
}

void Orientation::calc_forward() {
	forward_ = glm::mat2{
		axes_[0][0], axes_[1][0],
		axes_[0][1], axes_[1][1]
	};
}

bool operator==(Hex lhs, Hex rhs) {
	return (lhs.q == rhs.q) && (lhs.r == rhs.r);
}

glm::vec2 hex_to_pixel(Layout layout, Hex h) {
	glm::vec2 result = glm::vec2(h.q, h.r) * layout.orientation.forward_matrix() * layout.size;
	return glm::vec2(result.x + layout.origin.x, result.y + layout.origin.y);
}

FractionalHex pixel_to_hex(Layout layout, glm::vec2 p) {
	glm::vec2 result = (p - layout.origin) / layout.size * layout.orientation.inverse_matrix();
	return FractionalHex(result.x, result.y, -result.x - result.y);
}

Hex add(Hex a, Hex b) {
	return Hex{ a.q + b.q, a.r + b.r, a.s + b.s };
}

Hex subtract(Hex a, Hex b) {
	return Hex{ a.q - b.q, a.r - b.r, a.s - b.s };
}

Hex multiply(Hex a, int k) {
	return Hex{ a.q * k, a.r * k, a.s * k };
}

int hex_length(Hex hex) {
	return int((abs(hex.q) + abs(hex.r) + abs(hex.s)) / 2);
}

int hex_distance(Hex a, Hex b) {
	return hex_length(subtract(a, b));
}

Hex hex_direction(int direction) {
	return HEX_DIRECTIONS[direction];
}

Hex hex_neighbor(Hex hex, int direction) {
	return add(hex, hex_direction(direction));
}

Hex hex_round(FractionalHex h) {
	int q = int(round(h.q));
	int r = int(round(h.r));
	int s = int(round(h.s));
	double q_diff = abs(q - h.q);
	double r_diff = abs(r - h.r);
	double s_diff = abs(s - h.s);
	if (q_diff > r_diff && q_diff > s_diff) {
		q = -r - s;
	}
	else if (r_diff > s_diff) {
		r = -q - s;
	}
	else {
		s = -q - r;
	}
	return Hex{ q,r,s };
}

float lerp(double a, double b, double t) {
	return a * (1 - t) + b * t;
}

FractionalHex hex_lerp(Hex a, Hex b, double t) {
	return FractionalHex{
		lerp(a.q, b.q, t),
		lerp(a.r, b.r, t),
		lerp(a.s, b.s, t)
	};
}

std::vector<Hex> line(Hex a, Hex b) {
	int n = hex_distance(a, b);
	// nudge???
	std::vector<Hex> results = {};
	double step = 1.0 / std::max(n, 1);
	for (int i = 0; i <= n; ++i) {
		results.push_back(hex_round(hex_lerp(a, b, step * i)));
	}
	return results;
}