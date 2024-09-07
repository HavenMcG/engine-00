#pragma once
#include "Component.h"
#include "../Entity/Entity.h"
#include "../../Error/Error.h"
#include <unordered_map>
#include <unordered_set>
#include <expected>
#include <string>

class ParentCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<void, ErrorCode> add_component(Entity e, Entity parent);
	std::expected<void, ErrorCode> destroy_component(Entity e);

	std::unordered_map<Entity, EntityType> indices_;
	std::vector<Entity> owners_;
	std::vector<Entity> parents_;
	std::vector<int> depths_;

	EntityType n_ = 0;
	void binary_insertion_sort();
	std::unordered_set<Entity> unsorted_components_;
	static const int MAX_OPTIMAL_DEPTH = 5;
	int key_boundaries_[MAX_OPTIMAL_DEPTH] = { 0,0,0,0,0 };

	void print();
};

class ChildListCollection {
public:
	size_t size() const;
	std::expected<EntityType, ErrorCode> index(Entity e) const;
	bool has_component(Entity e) const;
	std::expected<void, ErrorCode> add_component(Entity e, std::unordered_set<Entity> children);
	std::expected<void, ErrorCode> destroy_component(Entity e);

	std::unordered_map<Entity, EntityType> indices_;
	std::vector<Entity> owners_;
	std::vector<std::unordered_set<Entity>> child_lists_;

	EntityType n_ = 0;
};

class RelationCollection {
public:
	std::expected<void, ErrorCode> make_child(Entity e, Entity parent);

	ParentCollection pc;
	ChildListCollection cc;

	void update_child_depths(Entity e);
	void sever(Entity e);
	void sever_children(Entity e);

	void print(Entity e) const;
};