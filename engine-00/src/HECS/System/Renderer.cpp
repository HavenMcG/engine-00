#include "Renderer.h"
#include <glad/glad.h>

const int MAX_DIFFUSE_TEXTURES = 1;
const int MAX_SPECULAR_TEXTURES = 1;

void Renderer::draw_models(ModelManager& model_m, Transform3dManager& transform_m, MeshManager& mesh_m, TextureManager& texture_m, Shader& shader) {
	// we need to find all entities with both a model and a transform
	// there will be fewer entities with models so we'll start there
	for (int model_index = 0; model_index < model_m.size(); ++model_index) {
		// check if corresponding position component exists:
		Entity e = model_m.owners()[model_index];
		auto transform = transform_m.get_component(e);
		if (transform.has_value()) {
			const Model& model = model_m.models()[model_index];

			glm::mat4 model_matrix = glm::mat4(1.0f);
			// translate:
			model_matrix = glm::translate(model_matrix, transform->position);
			// rotate:
			model_matrix *= glm::mat4_cast(transform->rotation);

			// set model matrix uniform:
			unsigned int model_matrix_loc = glGetUniformLocation(shader.id(), "model");
			glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix)); // args: uniform location, num matrices, transpose matrix, actual data

			for (int mesh_mat_index = 0; mesh_mat_index < model.meshes.size(); ++mesh_mat_index) {
				// check loaded here?

				const Mesh& mesh = model.meshes[mesh_mat_index];
				const Material& material = model.materials[mesh_mat_index];

				unsigned int id;
				int total_textures = 0;
				for (int i = 0; i < material.diffuses.size() && i <= MAX_DIFFUSE_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					texture_m.id(material.diffuses[0].path, id);
					shader.set_int("material.texture_diffuse1", 0); // temp
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.speculars.size() && i <= MAX_SPECULAR_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					texture_m.id(material.speculars[0].path, id);
					shader.set_int("material.texture_specular1", 1); // temp
					glBindTexture(GL_TEXTURE_2D, id);
				}

				// draw mesh
				glBindVertexArray(mesh_m.info(mesh).vao);
				glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
			}
		}
	}
}