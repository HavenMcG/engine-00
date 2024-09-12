#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

out vec3 frag_pos;
out vec3 normal;
out vec3 point_light_positions[12];
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

uniform int num_lights;
uniform vec3 point_light_world_positions[12];
uniform vec3 spotlight_world_positions[12];

void main() {
   frag_pos = vec3(view * model * vec4(a_pos, 1.0));
   normal = normal_matrix * a_normal;
   for (int i = 0; i < num_lights; i++) {
      point_light_positions[i] = vec3(view * vec4(point_light_world_positions[i], 1.0));
   }
   tex_coords = a_tex_coords;

   gl_Position = projection * view * model * vec4(a_pos, 1.0);
}