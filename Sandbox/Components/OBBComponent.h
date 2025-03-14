/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Liu Yaoting (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   OBBComponent.h
@brief:  This header file includes the implementation of the OBB component used by
		 ECS to handle and PhysicsCollisionSystem to handle collision detection.

		 Liu Yaoting (yaoting.loi): declared the struct component
									100%
*//*___________________________________________________________________________-*/

#pragma once
#include "vector2D.h"
#include "Force.h"

struct OBBComponent
{
	myMath::Vector2D center;      // Center position
	myMath::Vector2D halfExtents; // Half-width and half-height
	float rotation;				  // Rotation in radians
	myMath::Vector2D axes[2];     // Local axes (normalized)
};