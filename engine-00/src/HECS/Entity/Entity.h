#pragma once
#include <vector>
#include <deque>

const unsigned int ENTITY_INDEX_BITS = 24;
const unsigned int ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;

const unsigned int ENTITY_GENERATION_BITS = 8;
const unsigned int ENTITY_GENERATION_MASK = (1 << ENTITY_INDEX_BITS) - 1;

struct Entity {
	unsigned int id;
	unsigned int index() const;
	unsigned char generation() const;

	Entity(unsigned int index, unsigned char generation);
	Entity();
};

unsigned int calc_entity_id(unsigned int index, unsigned char generation);

using EntityType = unsigned int;
//const EntityType EMPTY_COMPONENT_INDEX = -1;

namespace std {
	template <> struct hash<Entity> {
		size_t operator()(Entity e) const {
			return hash<EntityType>()(e.id);
		}
	};
}

bool operator==(Entity lhs, Entity rhs);

class EntityCollection {
public:
	Entity create_entity();
	bool alive(Entity e);
	void destroy(Entity e);
private:
	std::vector<unsigned char> generations_;
	std::deque<unsigned int> free_indices_;
};