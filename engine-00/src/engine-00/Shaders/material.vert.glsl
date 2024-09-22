#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_tangent;
layout (location = 3) in vec2 a_tex_coords;

out vec3 frag_pos; // in tangent space
// we don't need to pass a view pos because we are working in view space
out vec3 normal;
out vec3 point_light_positions[12]; // in tangent space
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

uniform int num_point_lights;
uniform vec3 point_light_world_positions[12];
uniform int num_spotlights;
uniform vec3 spotlight_world_positions[12];

void main() {
   vec3 bitangent = cross(a_normal, a_tangent);
   vec3 T = normalize(vec3(normal_matrix * a_tangent));
   vec3 B = normalize(vec3(normal_matrix * bitangent));
   vec3 N = normalize(vec3(normal_matrix * a_normal));
   mat3 tbn = mat3(T, B, N); // the Tangent-Bitangent-Normal matrix for normal mapping
   mat3 tbni = transpose(tbn); // for an orthogonal matrix, the inverse equals the transpose. Transpose is faster than inverse.

   frag_pos = tbni * vec3(view * model * vec4(a_pos, 1.0));
   normal = tbni * N; // convert normal to tangent space
   for (int i = 0; i < num_point_lights; i++) {
      point_light_positions[i] = tbni * vec3(view * vec4(point_light_world_positions[i], 1.0));
   }
   tex_coords = a_tex_coords;

   gl_Position = projection * view * model * vec4(a_pos, 1.0);
}