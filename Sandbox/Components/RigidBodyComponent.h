#pragma once
#include "vector2D.h"

struct RigidBodyComponent
{
	float speed;
	myMath::Vector2D velocity;
	myMath::Vector2D acceleration;
	float mass;
	myMath::Vector2D force;
	float gravityScale;

	RigidBodyComponent()
		: velocity(0.0f, 0.0f), acceleration(0.0f, 0.0f), mass(1.0f), force(0.0f, 0.0f), gravityScale(1.0f) {}
};