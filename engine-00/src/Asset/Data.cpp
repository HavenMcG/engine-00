#include "Data.h"

Cuboid generate_bounding_box(const MeshData& data) {
    glm::vec3 min_point = data.vertices[0].position;
    glm::vec3 max_point = data.vertices[0].position;
    for (int i = 1; i < data.vertices.size(); ++i) {

        const float& ix = data.vertices[i].position.x;
        if (ix < min_point.x) min_point.x = ix;
        else if (ix > max_point.x) max_point.x = ix;

        const float& iy = data.vertices[i].position.y;
        if (iy < min_point.y) min_point.y = iy;
        else if (iy > max_point.y) max_point.y = iy;

        const float& iz = data.vertices[i].position.z;
        if (iz < min_point.z) min_point.z = iz;
        else if (iz > max_point.z) max_point.z = iz;
    }
    return Cuboid{ min_point,max_point };
}

glm::vec3 Cuboid::size() {
    float width = std::abs(p1.x - p2.x);
    float height = std::abs(p1.y - p2.y);
    float depth = std::abs(p1.z - p2.z);
    return glm::vec3{ width, height, depth };
}

MeshData Cuboid::generate_mesh() {
    // Define the 8 unique corner vertices of the cube
    glm::vec3 v[8] = {
        {p1.x, p1.y, p1.z},
        {p1.x, p1.y, p2.z},
        {p1.x, p2.y, p1.z},
        {p1.x, p2.y, p2.z},
        {p2.x, p1.y, p1.z},
        {p2.x, p1.y, p2.z},
        {p2.x, p2.y, p1.z},
        {p2.x, p2.y, p2.z}
    };

    // Define normals for each face (front, back, left, right, top, bottom)
    glm::vec3 normals[6] = {
        {0.0f, 0.0f, 1.0f},   // Front face
        {0.0f, 0.0f, -1.0f},  // Back face
        {-1.0f, 0.0f, 0.0f},  // Left face
        {1.0f, 0.0f, 0.0f},   // Right face
        {0.0f, 1.0f, 0.0f},   // Top face
        {0.0f, -1.0f, 0.0f}   // Bottom face
    };

    glm::vec2 tex_coords[4] = {
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f}
    };

    // Define vertices and normals for each face (6 faces, 2 triangles per face, 3 vertices per triangle)
    std::vector<Vertex> vertices = {
        // Front face
        {v[1], normals[0]},
        {v[7], normals[0]},
        {v[5], normals[0]},

        {v[1], normals[0]},
        {v[3], normals[0]},
        {v[7], normals[0]},

        // Back face
        {v[0], normals[1]},
        {v[6], normals[1]},
        {v[2], normals[1]},

        {v[0], normals[1]},
        {v[5], normals[1]},
        {v[6], normals[1]},

        // Left face
        {v[0], normals[2]},
        {v[3], normals[2]},
        {v[1], normals[2]},

        {v[0], normals[2]},
        {v[2], normals[2]},
        {v[3], normals[2]},

        // Right face
        {v[4], normals[3]},
        {v[7], normals[3]},
        {v[6], normals[3]},

        {v[4], normals[3]},
        {v[5], normals[3]},
        {v[7], normals[3]},

        // Top face
        {v[2], normals[4]},
        {v[7], normals[4]},
        {v[3], normals[4]},

        {v[2], normals[4]},
        {v[6], normals[4]},
        {v[7], normals[4]},

        // Bottom face
        {v[0], normals[5]},
        {v[5], normals[5]},
        {v[4], normals[5]},

        {v[0], normals[5]},
        {v[1], normals[5]},
        {v[5], normals[5]}
    };

    vertices = {
        // Back face
        {{p1.x,p1.y,p1.z}, normals[1]},
        {{p2.x,p2.y,p1.z}, normals[1]},
        {{p2.x,p1.y,p1.z}, normals[1]},

        {{p2.x,p2.y,p1.z}, normals[1]},
        {{p1.x,p1.y,p1.z}, normals[1]},
        {{p1.x,p2.y,p1.z}, normals[1]},

        // Front face
        {{p1.x,p1.y,p2.z}, normals[0]},
        {{p2.x,p1.y,p2.z}, normals[0]},
        {{p2.x,p2.y,p2.z}, normals[0]},

        {{p2.x,p2.y,p2.z}, normals[0]},
        {{p1.x,p2.y,p2.z}, normals[0]},
        {{p1.x,p1.y,p2.z}, normals[0]},

        // Left f.yace.z
        {{p1.x,p2.y,p2.z}, normals[2]},
        {{p1.x,p2.y,p1.z}, normals[2]},
        {{p1.x,p1.y,p1.z}, normals[2]},

        {{p1.x,p1.y,p1.z}, normals[2]},
        {{p1.x,p1.y,p2.z}, normals[2]},
        {{p1.x,p2.y,p2.z}, normals[2]},

        // Right .yfac.ze
        {{p2.x,p2.y,p2.z}, normals[3]},
        {{p2.x,p1.y,p1.z}, normals[3]},
        {{p2.x,p2.y,p1.z}, normals[3]},

        {{p2.x,p1.y,p1.z}, normals[3]},
        {{p2.x,p2.y,p2.z}, normals[3]},
        {{p2.x,p1.y,p2.z}, normals[3]},

        // Bottom.y fa.zce
        {{p1.x,p1.y,p1.z}, normals[5]},
        {{p2.x,p1.y,p1.z}, normals[5]},
        {{p2.x,p1.y,p2.z}, normals[5]},

        {{p2.x,p1.y,p2.z}, normals[5]},
        {{p1.x,p1.y,p2.z}, normals[5]},
        {{p1.x,p1.y,p1.z}, normals[5]},

        // Top fa.yce .z
        {{p1.x,p2.y,p1.z}, normals[4]},
        {{p2.x,p2.y,p2.z}, normals[4]},
        {{p2.x,p2.y,p1.z}, normals[4]},

        {{p2.x,p2.y,p2.z}, normals[4]},
        {{p1.x,p2.y,p1.z}, normals[4]},
        {{p1.x,p2.y,p2.z}, normals[4]}

    };

    std::vector<unsigned int> indices{};
    for (unsigned int i = 0; i < 36; ++i) indices.push_back(i);

    return MeshData{ vertices,indices };
}

glm::vec3 Cuboid::center() {
    glm::vec3 s = size();
    glm::vec3 center{};
    center.x = std::min(p1.x, p2.x) + s.x / 2.0f;
    center.y = std::min(p1.y, p2.y) + s.y / 2.0f;
    center.z = std::min(p1.z, p2.z) + s.z / 2.0f;
    return center;
}