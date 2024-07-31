#include "SimpleRenderer.h"
#include "glad/glad.h"
#include <iostream>

void SimpleRenderer::draw_model(Model* model, Shader* shader, MeshManager& mm, TextureManager& tl) {
	for (unsigned int i = 0; i < model->meshes.size(); ++i) {
		draw_mesh(model->meshes[i], model->materials[i], shader, mm, tl);
	}
}

void SimpleRenderer::draw_mesh(const Mesh& mesh, const Material& material, Shader* shader, MeshManager& mm, TextureManager& tl) {
	// check loaded here?

	unsigned int id;
	int total_textures = 0;
	for (int i = 0; i < material.diffuses.size() && i <= 1; ++i, ++total_textures) { // temp
		glActiveTexture(GL_TEXTURE0 + total_textures);
		tl.id(material.diffuses[0].path, id);
		shader->set_int("material.texture_diffuse1", 0); // temp
		glBindTexture(GL_TEXTURE_2D, id);
	}
	
	for (int i = 0; i < material.speculars.size() && i <= 1; ++i, ++total_textures) { // temp
		glActiveTexture(GL_TEXTURE0 + total_textures);
		tl.id(material.speculars[0].path, id);
		shader->set_int("material.texture_specular1", 1); // temp
		glBindTexture(GL_TEXTURE_2D, id);
	}

	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(mm.info(mesh).vao);
	glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
}