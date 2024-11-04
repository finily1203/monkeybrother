#pragma once
#include "vector2D.h"

struct RigidBodyComponent
{
	float mass;
	myMath::Vector2D gravityScale;
	float jump;
	float dampening;
	myMath::Vector2D velocity;
	myMath::Vector2D acceleration;
	myMath::Vector2D force;
	myMath::Vector2D accumulatedForce;

	RigidBodyComponent() : mass(1.0f), gravityScale(0.0f, 0.0f), jump(0.0f), dampening(0.0f), 
						   velocity(0.0f, 0.0f), acceleration(0.0f, 0.0f), force(0.0f, 0.0f), 
						   accumulatedForce(0.0f, 0.0f) {}
};