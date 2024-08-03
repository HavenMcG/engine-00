#include "Renderer.h"
#include <glad/glad.h>

const int MAX_DIFFUSE_TEXTURES = 8;
const int MAX_SPECULAR_TEXTURES = 8;

void Renderer::draw_models(glm::mat4 view_matrix, Shader& shader, ModelManager& model_m, Transform3dManager& transform_m) {
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
			shader.set_mat4("model", model_matrix);

			// set normal matrix uniform
			glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(view_matrix * model_matrix)));
			shader.set_mat3("normal_matrix", normal_matrix);

			for (int mesh_mat_index = 0; mesh_mat_index < model.meshes.size(); ++mesh_mat_index) {
				// check loaded here?

				const Mesh& mesh = model.meshes[mesh_mat_index];
				const Material& material = model.materials[mesh_mat_index];

				shader.set_material("material", material, model_m.texture_m);

				unsigned int id;
				int total_textures = 0;
				for (int i = 0; i < material.diffuses.size() && i <= MAX_DIFFUSE_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = *model_m.texture_m.id(material.diffuses[0].texture.path);
					shader.set_int("material.diffuse_textures[" + std::to_string(i) + "]", total_textures);
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.speculars.size() && i <= MAX_SPECULAR_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = *model_m.texture_m.id(material.speculars[0].texture.path);
					shader.set_int("material.specular_textures[" + std::to_string(i) + "]", total_textures);
					glBindTexture(GL_TEXTURE_2D, id);
				}

				// draw mesh
				glBindVertexArray(model_m.mesh_m.info(mesh).vao);
				glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
			}
		}
	}
}