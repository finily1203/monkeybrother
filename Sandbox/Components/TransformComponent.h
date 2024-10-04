/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   TransformComponent.h
@brief:  This header file is the Transform Component used by the ECS to handle
		 the transformation of the game objects.
		 Joel Chu (c.weiyuan): declared the struct component
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include <glm/glm.hpp>

struct TransformComponent
{
	glm::vec2 orientation;
	glm::vec2 scale;
	glm::vec2 position;
};