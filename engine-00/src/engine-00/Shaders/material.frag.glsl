#version 400 core

const int MAX_TEXURES_PER_STACK = 4;

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

struct TextureStack {
    sampler2D textures[MAX_TEXURES_PER_STACK];
    int blend_strengths[MAX_TEXURES_PER_STACK];
    int blend_ops[MAX_TEXURES_PER_STACK];
    int num_textures;
};

struct Material {
	TextureStack diffuse_stack;
    TextureStack specular_stack;
    TextureStack emissive_stack;

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

vec3 blendTextures(vec3 base_color, TextureStack stack, vec2 tex_coords) {
    vec3 result = base_color;
    for (int i = 0; i < stack.num_textures; i++) {
        vec3 color = vec3(texture(stack.textures[i], tex_coords));
        color *= stack.blend_strengths[i];
        int blend_op = stack.blend_ops[i];
        if (blend_op == TEX_BLEND_ADD) {
            result += color;
        } else if (blend_op == TEX_BLEND_MUL) {
            result *= color;
        }
    }
    return result;
}

void main() {
    vec3 diffuse_color = blendTextures(material.diffuse_color, material.diffuse_stack, tex_coords); // note: emissive gamma correction special case??
    vec3 specular_color = blendTextures(material.specular_color, material.specular_stack, tex_coords);
    vec3 emissive = blendTextures(material.emissive_color, material.emissive_stack, tex_coords);

    // ambient
	vec3 ambient = light.ambient * diffuse_color;

    // diffuse
	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuse_color;

    // specular
	vec3 view_dir = normalize(-frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * specular_color;

    // attenuation
    float distance = length(light_pos - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

	vec3 result = ambient + diffuse + specular + emissive;
	frag_color = vec4(result, material.opacity);
}