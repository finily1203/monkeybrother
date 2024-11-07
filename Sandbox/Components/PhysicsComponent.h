#pragma once
#include "vector2D.h"
#include "Force.h"

struct PhysicsComponent
{
	float mass;
	myMath::Vector2D gravityScale;
	float jump;
	float dampening;
	myMath::Vector2D velocity;
	myMath::Vector2D acceleration;
	Force force;
	myMath::Vector2D accumulatedForce;
	float prevForce;
	float targetForce;

	PhysicsComponent() : mass(1.0f), gravityScale(0.0f, 0.0f), jump(0.0f), dampening(0.0f),
		velocity(0.0f, 0.0f), acceleration(0.0f, 0.0f), force(myMath::Vector2D(0.f, 0.f), 0.0f),
						   accumulatedForce(0.0f, 0.0f), prevForce(0.0f), targetForce(0.0f) {}
};
