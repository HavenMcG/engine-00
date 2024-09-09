#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <iomanip>

const int MAX_DIFFUSE_TEXTURES = 8;
const int MAX_SPECULAR_TEXTURES = 8;

void Renderer::draw_models(glm::mat4 view_matrix, Shader& shader, ModelCollection& models, TransformCollection& transforms, LightCollection& lights, const OglAssetStore& assets) {
	shader.use();
	// we need to find all entities with both a model and a transform
	// there will be fewer entities with models so we'll start there
	for (int model_index = 0; model_index < models.size(); ++model_index) {
		// check if corresponding position component exists:
		Entity e = models.owners()[model_index];
		auto transform = transforms.world_transform(e);
		if (transform.has_value()) {
			const Model& model = models.models()[model_index];

			glm::mat4 model_matrix = glm::mat4(1.0f);
			// translate:
			model_matrix = glm::translate(model_matrix, transform->position + transform->position_offset);
			// rotate:
			model_matrix *= glm::mat4_cast(transform->rotation);
			// scale:
			model_matrix *= glm::scale(glm::mat4(1.0f), transform->scale);

			/*std::cout << std::endl;
			std::cout << "Model Matrix: " << std::endl;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					std::cout << std::setw(8) << model_matrix[i][j] << " ";
				}
				std::cout << std::endl;
			}*/

			// set model matrix uniform:
			shader.set_mat4("model", model_matrix);

			// set normal matrix uniform
			glm::mat3 normal_matrix = glm::mat3(glm::inverseTranspose(view_matrix * model_matrix));
			shader.set_mat3("normal_matrix", normal_matrix);

			// temp?
			shader.set_vec3("light_pos_world", *transforms.position(lights.indices_.begin()->first));
			shader.set_light("light", *lights.get_component(lights.indices_.begin()->first));

			for (int mesh_mat_index = 0; mesh_mat_index < model.meshes.size(); ++mesh_mat_index) {
				// check loaded here?

				const Mesh& mesh = model.meshes[mesh_mat_index];
				const Material& material = model.materials[mesh_mat_index];

				shader.set_material("material", material);

				unsigned int id;
				int total_textures = 0;
				for (int i = 0; i < material.texture_diffuses.size() && i <= MAX_DIFFUSE_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					//shader.set_int("material.diffuse_textures[" + std::to_string(i) + "]", total_textures);
					id = assets.tex_ogl_ids_[material.diffuses[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.texture_speculars.size() && i <= MAX_SPECULAR_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					//shader.set_int("material.specular_textures[" + std::to_string(i) + "]", total_textures);
					id = assets.tex_ogl_ids_[material.speculars[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				// draw mesh
				glBindVertexArray(assets.mesh_vaos_[mesh.index()]);
				glDrawElements(GL_TRIANGLES, assets.mesh_index_counts_[mesh.index()], GL_UNSIGNED_INT, 0);
			}
		}
	}
}

void Renderer::draw_ui(Shader& shader, ModelCollection& models, TransformCollection& transforms, const OglAssetStore& assets) {
	shader.use();
	// we need to find all entities with both a model and a transform
	// there will be fewer entities with models so we'll start there
	for (int model_index = 0; model_index < models.size(); ++model_index) {
		// check if corresponding position component exists:
		Entity e = models.owners()[model_index];
		auto transform = transforms.world_transform(e);
		if (transform.has_value()) {
			const Model& model = models.models()[model_index];

			glm::mat4 view_matrix = glm::mat4(1.0f);
			shader.set_mat4("view", view_matrix);
			glm::mat4 model_matrix = glm::mat4(1.0f);
			// translate:
			model_matrix = glm::translate(model_matrix, transform->position);
			// rotate:
			model_matrix *= glm::mat4_cast(transform->rotation);

			// set model matrix uniform:
			shader.set_mat4("model", model_matrix);

			glm::mat4 projection_matrix = glm::mat4(1.0f);
			shader.set_mat4("projection", projection_matrix);

			// set normal matrix uniform
			glm::mat3 normal_matrix = glm::mat3(glm::inverseTranspose(view_matrix * model_matrix));
			shader.set_mat3("normal_matrix", normal_matrix);

			for (int mesh_mat_index = 0; mesh_mat_index < model.meshes.size(); ++mesh_mat_index) {
				// check loaded here?

				const Mesh& mesh = model.meshes[mesh_mat_index];
				const Material& material = model.materials[mesh_mat_index];

				shader.set_material("material", material);

				unsigned int id;
				int total_textures = 0;
				for (int i = 0; i < material.texture_diffuses.size() && i <= MAX_DIFFUSE_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					shader.set_int("material.diffuse_textures[" + std::to_string(i) + "]", total_textures);
					id = assets.tex_ogl_ids_[material.texture_diffuses[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.texture_speculars.size() && i <= MAX_SPECULAR_TEXTURES; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					shader.set_int("material.specular_textures[" + std::to_string(i) + "]", total_textures);
					id = assets.tex_ogl_ids_[material.texture_speculars[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				// draw mesh
				glBindVertexArray(assets.mesh_vaos_[mesh.index()]);
				glDrawElements(GL_TRIANGLES, assets.mesh_index_counts_[mesh.index()], GL_UNSIGNED_INT, 0);
			}
		}
	}
}
