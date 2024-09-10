#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <iomanip>

void Renderer::draw_models(glm::mat4 view_matrix, Shader& shader, ModelCollection& models, TransformCollection& transforms, DirectionalLightCollection& directional_lights, PointLightCollection& point_lights, const OglAssetStore& assets) {
	shader.use();

	// find directional lights:
	int light_count = 0;
	for (int i = 0; i < directional_lights.size(); ++i) {
		Entity e = directional_lights.owners_[i];
		shader.set_directional_light("directional_lights[" + std::to_string(light_count) + "]", *directional_lights.get_component(e));
		++light_count;
	}
	shader.set_int("num_directional_lights", light_count);

	// find point lights:
	light_count = 0;
	for (int i = 0; i < point_lights.size(); ++i) {
		Entity e = point_lights.owners_[i];
		if (transforms.has_component(e)) {
			shader.set_vec3("point_light_world_positions[" + std::to_string(light_count) + "]", *transforms.position(e));
			shader.set_point_light("point_lights[" + std::to_string(light_count) + "]", *point_lights.get_component(e));
			++light_count;
		}
		shader.set_int("num_point_lights", light_count);
	}

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

			// set model matrix uniform:
			shader.set_mat4("model", model_matrix);

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
				for (int i = 0; i < material.texture_diffuses.size() && i < MAX_TEXTURES_PER_STACK; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = assets.tex_ogl_ids_[material.texture_diffuses[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.texture_speculars.size() && i < MAX_TEXTURES_PER_STACK; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = assets.tex_ogl_ids_[material.texture_speculars[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.texture_emissives.size() && i < MAX_TEXTURES_PER_STACK; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = assets.tex_ogl_ids_[material.texture_emissives[i].texture.index()];
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
				for (int i = 0; i < material.texture_diffuses.size() && i < MAX_TEXTURES_PER_STACK; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = assets.tex_ogl_ids_[material.texture_diffuses[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.texture_speculars.size() && i < MAX_TEXTURES_PER_STACK; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = assets.tex_ogl_ids_[material.texture_speculars[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				for (int i = 0; i < material.texture_emissives.size() && i < MAX_TEXTURES_PER_STACK; ++i, ++total_textures) {
					glActiveTexture(GL_TEXTURE0 + total_textures);
					id = assets.tex_ogl_ids_[material.texture_emissives[i].texture.index()];
					glBindTexture(GL_TEXTURE_2D, id);
				}

				// draw mesh
				glBindVertexArray(assets.mesh_vaos_[mesh.index()]);
				glDrawElements(GL_TRIANGLES, assets.mesh_index_counts_[mesh.index()], GL_UNSIGNED_INT, 0);
			}
		}
	}
}
