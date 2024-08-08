#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 area = vec2(100.0, 100.0);

out vec3 FragPos;

void main()
{
    FragPos = vec3(aPos.x * area.x, aPos.y, aPos.z * area.y);
    gl_Position = projection * view * model * vec4(aPos.x * area.x, aPos.y, aPos.z * area.y, 1.0);
}
