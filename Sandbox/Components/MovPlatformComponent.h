/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   MovPlatformComponent.h
@brief:  This header file includes the implementation of the Moving Platform Component
		 to be used by ECS to have moving platforms within the game. This component
		 will be used by its respective behavior system to handle the logic of the
		 moving platform.

		 Joel Chu (c.weiyuan): declared the struct component
							   100%
*//*___________________________________________________________________________-*/
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