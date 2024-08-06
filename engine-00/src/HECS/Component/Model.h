#pragma once
#include "../Entity/Entity.h"
#include "../../Asset/Storing/OpenGL/OglAssetStore.h"
#include <unordered_map>
#include <expected>
#include <string>
#include "../../Asset/Mesh.h"
#include "../../Asset/Material.h"

struct Model {
	std::string name; // for models loaded from files, the name is the model's filepath
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};

class ModelCollection {
public:
	size_t size();
	const std::vector<Entity>& owners();
	const std::vector<Model>& models();
	std::expected<void, bool> add_component(Entity e, Model& model);

private:
	std::unordered_map<Entity, EntityType> map_; // map_[0] would contain the index of the actual component data for Entity 0

	std::vector<Entity> owners_; // we store the owning entities so we don't have to slow search the map_
	std::vector<Model> models_;
};