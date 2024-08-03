#version 330 core

const int MAX_DIFFUSE_TEXTURES = 8;
const int MAX_SPECULAR_TEXTURES = 8;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	sampler2D diffuse_textures[MAX_DIFFUSE_TEXTURES];
    int diffuse_blend_strengths[MAX_DIFFUSE_TEXTURES];
    int num_diffuse_textures;

	sampler2D specular_textures[MAX_SPECULAR_TEXTURES];
    int specular_blend_strengths[MAX_SPECULAR_TEXTURES];
    int num_specular_textures;

    vec3 diffuse_color;
    vec3 specular_color;
	float shininess;
};

in vec3 frag_pos;
in vec3 normal;
in vec3 light_pos;
in vec2 tex_coords;

out vec4 frag_color;

uniform Light light;
uniform Material material;

void main() {
	vec3 ambient = light.ambient * vec3(texture(material.diffuse_textures[0], tex_coords));

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse_textures[0], tex_coords));

	vec3 view_dir = normalize(-frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular_textures[0], tex_coords));

	vec3 result = ambient + diffuse + specular;
	frag_color = vec4(result, 1.0);
}