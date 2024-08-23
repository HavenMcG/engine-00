#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <unordered_map>
#include <unordered_set>

template <typename Number>
struct Hex_ { // Both storage types, both constructors
	union {
		Number v[3];
		struct { Number q, r, s; };
	};

	Hex_(Number q_, Number r_) : v{ q_, r_, -q_ - r_ } {}
	Hex_(Number q_, Number r_, Number s_) : v{ q_, r_, s_ } {}
};
typedef Hex_<int> Hex;
typedef Hex_<float> FractionalHex;

template <typename Number>
bool operator==(Hex_<Number> lhs, Hex_<Number> rhs) {
	return (lhs.q == rhs.q) && (lhs.r == rhs.r);
}

const glm::mat3x2 AXES{
	3.0f/2.0f, sqrt(3.0f)/2.0f,
	0.0f, sqrt(3.0f),
	-3.0f/2.0f, sqrt(3.0f)/2.0f
};

struct Orientation {
	float angle() const;
	glm::mat3x2 axes() const;
	glm::mat2 forward_matrix() const;
	glm::mat2 inverse_matrix() const;
	Orientation(float angle);
private:
	float angle_;
	glm::mat3x2 axes_;
	glm::mat2 forward_;
	glm::mat2 inverse_;
	void calc_axes();
	void calc_forward();
};

const Orientation pointy = Orientation{ 
	-0.5f
};
const Orientation flat = Orientation{
	0.0f
};

struct Layout {
	const Orientation orientation;
	const glm::vec2 size;
	const glm::vec2 origin;
};

glm::vec2 hex_to_cartesian(Layout layout, Hex h);

FractionalHex cartesian_to_hex(Layout layout, glm::vec2 p);

Hex add(Hex a, Hex b);

Hex subtract(Hex a, Hex b);

Hex multiply(Hex a, int k);

int hex_length(Hex hex);

int hex_distance(Hex a, Hex b);

const Hex HEX_DIRECTIONS[] = {
	Hex{1,0,-1}, Hex{1,-1,0}, Hex{0,-1,1},
	Hex{-1,0,1}, Hex{-1,1,0}, Hex{0,1,-1}
};

Hex hex_direction(int direction);

Hex hex_neighbor(Hex hex, int direction);

Hex hex_round(FractionalHex h);

float lerp(double a, double b, double t);

FractionalHex hex_lerp(Hex a, Hex b, double t);

std::vector<Hex> line(Hex a, Hex b);

namespace std {
	template <> struct hash<Hex> {
		size_t operator()(const Hex& h) const {
			size_t hq = hash<int>()(h.q);
			size_t hr = hash<int>()(h.r);
			return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
		}
	};
}

template<typename T>
std::unordered_map<Hex, T> parallelogram_map(int q_start, int q_end, int r_start, int r_end) {
	std::unordered_map<Hex, T> map;
	for (int q = q_start; q <= q_end; ++q) {
		for (int r = r_start; r <= r_end; ++r) {
			map.emplace(Hex{ q, r, -q - r }, T{});
		}
	}
	return map;
}

template<typename T>
std::unordered_map<Hex, T> triangle_map(int q_start, int r_start, int size, int orientation) {
	std::unordered_map<Hex, T> map;
	for (int q = q_start; q <= size; ++q) {
		for (int r = 0; r <= size - q; ++r) {
			map.emplace(Hex{ q,r,-q - r }, T{});
		}
	}
	return map;
}
template<typename T>
std::unordered_map<Hex, T> hexagon_map(int radius) {
	std::unordered_map<Hex, T> map;
	for (int q = -radius; q <= radius; ++q) {
		int r1 = std::max(-radius, -q - radius);
		int r2 = std::min(radius, -q + radius);
		for (int r = r1; r <= r2; ++r) {
			map.emplace(Hex{ q,r,-q - r }, T{});
		}
	}
	return map;
}

template<typename T>
std::unordered_map<Hex, T> rectangle_map_pointy(int left, int right, int top, int bottom) {
	std::unordered_map<Hex, T> map;
	for (int r = top; r <= bottom; ++r) {
		int r_offset = floor(r / 2.0);
		for (int q = left - r_offset; q <= right - r_offset; ++q) {
			map.emplace(Hex{ q,r,-q - r }, T{});
		}
	}
	return map;
}

template<typename T>
std::unordered_map<Hex, T> rectangle_map_flat(int left, int right, int top, int bottom) {
	std::unordered_map<Hex, T> map;
	for (int q = left; q <= right; ++q) {
		int q_offset = floor(q / 2.0);
		for (int r = top - q_offset; r <= bottom - q_offset; ++r) {
			map.emplace(Hex{ q,r,-q - r }, T{});
		}
	}
	return map;
}

std::unordered_set<Hex> parallelogram_set(int q_start, int q_end, int r_start, int r_end);
std::unordered_set<Hex> triangle_set(int q_start, int r_start, int size, int orientation);
std::unordered_set<Hex> hexagon_set(int radius);
std::unordered_set<Hex> rectangle_set_pointy(int left, int right, int top, int bottom);
std::unordered_set<Hex> rectangle_set_flat(int left, int right, int top, int bottom);