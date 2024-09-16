#include "Material.h"
#include <iostream>

void print(Material m) {
	std::cout << "Diffuse Textures:" << std::endl;
	for (auto t : m.texture_diffuses) {
		std::cout << "\t" << t.texture.id << " | " << t.blend_op << " | " << t.blend_strength << std::endl;
	}
	std::cout << "Specular Textures:" << std::endl;
	for (auto t : m.texture_speculars) {
		std::cout << "\t" << t.texture.id << " | " << t.blend_op << " | " << t.blend_strength << std::endl;
	}
	std::cout << "Emissive Textures:" << std::endl;
	for (auto t : m.texture_emissives) {
		std::cout << "\t" << t.texture.id << " | " << t.blend_op << " | " << t.blend_strength << std::endl;
	}
	std::cout << "Diffuse Color: " << m.color_diffuse.r << ", " << m.color_diffuse.g << ", " << m.color_diffuse.b << std::endl;
	std::cout << "Specular Color: " << m.color_specular.r << ", " << m.color_specular.g << ", " << m.color_specular.b << std::endl;
	std::cout << "Emissive Color: " << m.color_emissive.r << ", " << m.color_emissive.g << ", " << m.color_emissive.b << std::endl;
	std::cout << "Shininess: " << m.shininess << std::endl;
	std::cout << "Opacity: " << m.opacity << std::endl;
}
