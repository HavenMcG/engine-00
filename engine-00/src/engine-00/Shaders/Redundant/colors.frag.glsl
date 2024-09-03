#version 330 core

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
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
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, tex_coords));

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, tex_coords));

	vec3 view_dir = normalize(-frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, tex_coords));

	vec3 result = ambient + diffuse + specular;
	frag_color = vec4(result, 1.0);
}