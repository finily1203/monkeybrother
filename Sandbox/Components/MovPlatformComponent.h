#pragma once

#include "vector2D.h"

struct MovPlatformComponent
{
	float speed;
	float maxDistance;
	bool movForward;

	myMath::Vector2D startPos;
	myMath::Vector2D direction;

	MovPlatformComponent() : speed(0.f), direction(0.0f, 0.0f), maxDistance(0.0f), startPos(0.0f,0.0f), movForward(true) {}
};