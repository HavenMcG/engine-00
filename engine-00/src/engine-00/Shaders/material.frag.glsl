#version 400 core

const int MAX_TEXURES_PER_STACK = 4;

// Texture blending operations
const int TEX_BLEND_ADD = 0;
const int TEX_BLEND_MUL = 1;

struct DirectionalLight {
    vec3 color;
    vec3 direction;
};

struct PointLight {
	vec3 color;
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
in vec2 tex_coords;
in vec3 point_light_positions[12];

out vec4 frag_color;

uniform DirectionalLight[6] directional_lights;
uniform int num_directional_lights;

uniform PointLight[12] point_lights;
uniform int num_point_lights;

uniform Material material;

vec3 calc_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular);
vec3 calc_point_light(PointLight light, vec3 light_pos, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular);

vec3 blendTextures(vec3 base_color, TextureStack stack, vec2 tex_coords);

void main() {
    vec3 diffuse_color = blendTextures(material.diffuse_color, material.diffuse_stack, tex_coords);
    vec3 ambient_color = diffuse_color;
    vec3 specular_color = blendTextures(material.specular_color, material.specular_stack, tex_coords);
    vec3 emissive = blendTextures(material.emissive_color, material.emissive_stack, tex_coords); // note: emissive gamma correction special case??

	vec3 norm = normalize(normal);
	vec3 view_dir = normalize(-frag_pos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < num_directional_lights; i++) {
        result += calc_directional_light(directional_lights[i], norm, view_dir, ambient_color, diffuse_color, specular_color);
    }
    for (int i = 0; i < num_point_lights; i++) {
        result += calc_point_light(point_lights[i], point_light_positions[i], norm, view_dir, frag_pos, ambient_color, diffuse_color, specular_color);
    }
	result += emissive;
	frag_color = vec4(result, material.opacity);
}

vec3 calc_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 light_dir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    // combine results
    vec3 ambient = 0.7 * light.color * mat_ambient;
    vec3 diffuse = diff * light.color * mat_diffuse;
    vec3 specular = spec * light.color * mat_specular;
    return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 light_pos, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 light_dir = normalize(light_pos - frag_pos);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    // attenuation
    float distance = length(light_pos - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = 0.7 * light.color * mat_ambient;
    vec3 diffuse = diff * light.color * mat_diffuse;
    vec3 specular = spec * light.color * mat_specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

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