#include "Entity.h"

unsigned int calc_entity_id(unsigned int index, unsigned char generation) {
	unsigned int id = 0;
	id |= (index & ENTITY_INDEX_MASK);
	id |= (generation & ENTITY_GENERATION_MASK) << ENTITY_INDEX_BITS;
	return id;
}

bool operator==(Entity lhs, Entity rhs) {
	return lhs.id == rhs.id;
}

Entity::Entity(unsigned int index, unsigned char generation) {
	id = calc_entity_id(index, generation);
}

Entity::Entity() : id{ 0 } {}

unsigned int Entity::index() const {
	return id & ENTITY_INDEX_MASK;
}

unsigned char Entity::generation() const {
	return (id >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK;
}

//=================================================================================================

Entity EntityCollection::create_entity() {
	unsigned int index;
	if (free_indices_.empty()) {
		index = generations_.size();
		generations_.push_back(0);
	}
	else {
		index = free_indices_.front();
		free_indices_.pop_front();
	}
	return Entity{ index, generations_[index] };
}

bool EntityCollection::alive(Entity e) {
	return generations_[e.index()] == e.generation();
}

void EntityCollection::destroy(Entity e) {
	unsigned int i = e.index();
	++generations_[i];
	free_indices_.push_back(i);
}
