#version 330 core

struct Hex {
    int q, r, s;
};

struct FractionalHex {
    float q, r, s;
};

bool hex_equal(Hex lhs, Hex rhs) {
    return (lhs.q == rhs.q) && (lhs.r == rhs.r);
}

struct Orientation {
    mat2 forward;
    mat2 inverse;
    float start_angle;
};

struct HexLayout {
    Orientation orientation;
    vec2 size;
    vec2 origin;
};

vec2 hex_to_pixel(HexLayout l, Hex h) {
    vec2 result = vec2(h.q, h.r) * l.orientation.forward * l.size;
    return vec2(result.x + l.origin.x, result.y + l.origin.y);
}

FractionalHex pixel_to_hex(HexLayout l, vec2 p) {
    vec2 result = (p - l.origin) / l.size * l.orientation.inverse;
    return FractionalHex(result.x, result.y, -result.x - result.y);
}

Hex add(Hex a, Hex b) {
    return Hex(a.q + b.q, a.r + b.r, a.s + b.s);
}

Hex subtract(Hex a, Hex b) {
    return Hex(a.q - b.q, a.r - b.r, a.s - b.s);
}

Hex multiply(Hex a, int k) {
    return Hex(a.q * k, a.r * k, a.s * k);
}

int hex_length(Hex hex) {
    return int((abs(hex.q) + abs(hex.r) + abs(hex.s)) / 2);
}

int hex_distance(Hex a, Hex b) {
    return hex_length(subtract(a, b));
}

// Constants for hex directions
const Hex HEX_DIRECTIONS[6] = Hex[6](
    Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
    Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
);

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
    float q_diff = abs(q - h.q);
    float r_diff = abs(r - h.r);
    float s_diff = abs(s - h.s);
    if (q_diff > r_diff && q_diff > s_diff) {
        q = -r - s;
    } else if (r_diff > s_diff) {
        r = -q - s;
    } else {
        s = -q - r;
    }
    return Hex(q, r, s);
}

float lerp(float a, float b, float t) {
    return a * (1.0 - t) + b * t;
}

FractionalHex hex_lerp(Hex a, Hex b, float t) {
    return FractionalHex(
        lerp(a.q, b.q, t),
        lerp(a.r, b.r, t),
        lerp(a.s, b.s, t)
    );
}

void line(Hex a, Hex b, out Hex line[1000], out int line_length) {
    int n = hex_distance(a, b);
    float h_step = 1.0 / max(n, 1);
    line_length = 0;
    for (int i = 0; i <= n && i < 1000; ++i) {
        line[i] = hex_round(hex_lerp(a, b, h_step * i));
        ++line_length;
    }
}

float distance_from_center(mat3x2 axes, float hex_radius, vec2 local_point) {
    float max_r = 0.0;
    for (int i = 0; i < 3; i++) {
        float r = dot(local_point, normalize(axes[i]));
        r /= (sqrt(3) * 0.5 * hex_radius);
        max_r = max(max_r, abs(r));
    }
    return max_r;
}

in vec3 FragPos;

uniform HexLayout hex_layout;
uniform mat3x2 axes;
uniform vec4 color;
uniform float power = 8.0;

out vec4 FragColor;

void main() {
    vec2 uv = FragPos.xz;
    float radius = hex_layout.size.x;

    vec2 local_center = hex_to_pixel(hex_layout, hex_round(pixel_to_hex(hex_layout, uv)));
    vec2 local_coords = uv - local_center;

    float d = distance_from_center(axes, radius, local_coords);
    if (d <= 1.0) {
        float rad = pow(d, power);
        FragColor = vec4(color.rgb, rad);
    }
    else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}