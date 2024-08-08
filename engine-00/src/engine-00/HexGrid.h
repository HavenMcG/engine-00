#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

template <typename Number, int w>
struct Hex_ { // Both storage types, both constructors
	union {
		const Number v[3];
		struct { const Number q, r, s; };
	};

	Hex_(Number q_, Number r_) : v{ q_, r_, -q_ - r_ } {}
	Hex_(Number q_, Number r_, Number s_) : v{ q_, r_, s_ } {}
};
typedef Hex_<int, 1> Hex;
typedef Hex_<int, 0> HexDifference;
typedef Hex_<float, 1> FractionalHex;
typedef Hex_<float, 0> FractionalHexDifference;

struct Orientation {
	const glm::mat2 forward;
	const glm::mat2 inverse;
	const float start_angle;
};

const Orientation pointy = Orientation{ glm::mat2{ sqrt(3.0f), sqrt(3.0f) / 2.0f, 0.0f, 3.0f / 2.0f }, glm::mat2{ sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f }, 0.5f };
const Orientation flat = Orientation{ glm::mat2{ 3.0f / 2.0f, 0.0f, sqrt(3.0f) / 2.0f, sqrt(3.0) }, glm::mat2{ 2.0f / 3.0f, 0.0f, -1.0f / 3.0f, sqrt(3.0f) / 3.0f }, 0.0f };

struct Layout {
	const Orientation orientation;
	const glm::vec2 size;
	const glm::vec2 origin;
};

glm::vec2 hex_to_pixel(Layout layout, Hex h) {
	const Orientation& m = layout.orientation;
	float x = (m.forward[0][0] * h.q + m.forward[0][1] * h.r) * layout.size.x;
	float y = (m.forward[1][0] * h.q + m.forward[1][1] * h.r) * layout.size.y;
	return glm::vec2{ x + layout.origin.x, y + layout.origin.y };
}

FractionalHex pixel_to_hex(Layout layout, glm::vec2 p) {
	const Orientation& m = layout.orientation;
	glm::vec2 pt{ (p.x - layout.origin.x) / layout.size.x, (p.y - layout.origin.y) / layout.size.y };
	float q = m.inverse[0][0] * pt.x + m.inverse[0][1] * pt.y;
	float r = m.inverse[1][0] * pt.x + m.inverse[1][1] * pt.y;
	return FractionalHex(q, r, -q - r);
}