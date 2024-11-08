/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Lee Jing Wen (jingwen.lee)
@team:   MonkeHood
@course: CSD2401
@file:   PhysicsComponent.h
@brief:  This source file contains struct for Physics Component to be used by ECS
		 and Physics and Collision Systems.
		 Lee Jing Wen (jingwen.lee): Declared the struct component
									 100%
*//*____________________________________________________________________________-*/
#pragma once
#include "vector2D.h"
#include "Force.h"

struct PhysicsComponent
{

	myMath::Vector2D velocity;
	myMath::Vector2D gravityScale;
	myMath::Vector2D acceleration;
	myMath::Vector2D accumulatedForce;
	float jump;
	float dampening;
	float mass;
	float maxVelocity;
	float maxAccumulatedForce;
	float prevForce;
	float targetForce;
	Force force;

	PhysicsComponent() : velocity(0.0f, 0.0f), gravityScale(0.0f, 0.0f), acceleration(0.0f, 0.0f),
		accumulatedForce(0.0f, 0.0f), jump(0.0f), dampening(0.0f), mass(1.0f),
		maxVelocity(0.0f), maxAccumulatedForce(0.0f), prevForce(0.0f), targetForce(0.0f),
		force(myMath::Vector2D(0.f, 0.f), 0.0f) {}
};