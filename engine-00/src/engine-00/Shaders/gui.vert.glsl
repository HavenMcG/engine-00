#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

out vec3 frag_pos;
out vec3 normal;
out vec3 light_pos;
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;
uniform vec3 light_pos_world;
void main() {
   frag_pos = vec3(view * model * vec4(a_pos, 1.0));
   normal = normal_matrix * a_normal;
   light_pos = vec3(view * vec4(light_pos_world, 1.0));
   tex_coords = a_tex_coords;

   gl_Position = projection * view * model * vec4(a_pos, 1.0);
}