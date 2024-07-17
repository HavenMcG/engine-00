#include "Renderer.h"
#include "ModelLoader.h"
#include "glad/glad.h"
#include "TextureLoader.h"
#include "iostream"

void Renderer::draw_model(Model* model, Shader* shader) {
	for (unsigned int i = 0; i < model->meshes.size(); ++i) {
		draw_mesh(&model->meshes[i], shader);
	}
}

void Renderer::draw_mesh(Mesh* mesh, Shader* shader) {
	ModelLoader ml; // TEMP
	TextureLoader tl;
	ml.ogl_load_mesh(mesh);
	unsigned int diffuse_nr = 1;
	unsigned int specular_nr = 1;
	for (unsigned int i = 0; i < mesh->textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		std::string name;
		if (i == 0) {
			name = "texture_diffuse1";
		}
		else if (i == 1) {
			name = "texture_specular1";
		}

		unsigned int id;
		bool exists = tl.find_id(mesh->textures[i], id);
		shader->set_int(("material." + name).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(ml.get_ogl_info(mesh).vao);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
