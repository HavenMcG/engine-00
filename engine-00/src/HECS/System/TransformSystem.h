#pragma once
#include "../Entity/Entity.h"
#include "../Component/Transform.h"
#include "../../Error/Error.h"
#include <expected>

class TransformSystem {
public:
	TransformCollection& transforms;
	std::expected<void, ErrorCode> rotate_degrees(Entity e, float pitch, float yaw, float roll);

	TransformSystem(TransformCollection& transforms);
};