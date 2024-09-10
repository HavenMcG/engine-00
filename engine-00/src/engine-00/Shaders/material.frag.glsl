#version 400 core

const int MAX_DIFFUSE_TEXTURES = 8;
const int MAX_SPECULAR_TEXTURES = 8;
const int MAX_EMISSIVE_TEXTURES = 8;

// Texture blending operations
const int TEX_BLEND_ADD = 0;
const int TEX_BLEND_MUL = 1;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
	sampler2D diffuse_textures[MAX_DIFFUSE_TEXTURES];
    int diffuse_blend_strengths[MAX_DIFFUSE_TEXTURES];
    int diffuse_blend_ops[MAX_DIFFUSE_TEXTURES];
    int num_diffuse_textures;

	sampler2D specular_textures[MAX_SPECULAR_TEXTURES];
    int specular_blend_strengths[MAX_SPECULAR_TEXTURES];
    int specular_blend_ops[MAX_SPECULAR_TEXTURES];
    int num_specular_textures;

    sampler2D emissive_textures[MAX_EMISSIVE_TEXTURES];
    int emissive_blend_strengths[MAX_EMISSIVE_TEXTURES];
    int emissive_blend_ops[MAX_EMISSIVE_TEXTURES];
    int num_emissive_textures;

    vec3 diffuse_color;
    vec3 specular_color;
    vec3 emissive_color;
	float shininess;
    float opacity;
};

in vec3 frag_pos;
in vec3 normal;
in vec3 light_pos;
in vec2 tex_coords;

out vec4 frag_color;

uniform Light light;
uniform Material material;

vec3 blendTextures(vec3 base_color, sampler2D textures[MAX_DIFFUSE_TEXTURES], int blend_strengths[MAX_DIFFUSE_TEXTURES], int blend_ops[MAX_DIFFUSE_TEXTURES], int num_textures, vec2 tex_coords) {
    vec3 result = base_color;
    for (int i = 0; i < num_textures; i++) {
        vec3 color = vec3(texture(textures[i], tex_coords));
        color *= blend_strengths[i];
        if (blend_ops[i] == TEX_BLEND_ADD) {
            result += color;
        } else if (blend_ops[i] == TEX_BLEND_MUL) {
            result *= color;
        }
    }
    return result;
}

void main() {
    vec3 diffuse_color = blendTextures(material.diffuse_color, material.diffuse_textures, material.diffuse_blend_strengths, material.diffuse_blend_ops, material.num_diffuse_textures, tex_coords);
	vec3 ambient = light.ambient * diffuse_color;

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuse_color;

	vec3 view_dir = normalize(-frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = blendTextures(material.specular_color, material.specular_textures, material.specular_blend_strengths, material.specular_blend_ops, material.num_specular_textures, tex_coords);
    //specular_color = vec3(1.0,1.0,1.0);
	vec3 specular = light.specular * spec * specular_color;

    // gamma correction special case??
    vec3 emissive = blendTextures(material.emissive_color, material.emissive_textures, material.emissive_blend_strengths, material.emissive_blend_ops, material.num_emissive_textures, tex_coords);

    //specular = vec3(0.0);
	vec3 result = ambient + diffuse + specular + emissive;
	frag_color = vec4(result, material.opacity);
}