#pragma once
#include <glm/glm.hpp>

struct TransformComponent
{
	glm::vec2 orientation;
	glm::vec2 position;
	glm::vec2 scale;
};