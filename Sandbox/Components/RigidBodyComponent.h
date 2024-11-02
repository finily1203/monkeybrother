#pragma once
#include "vector2D.h"

struct RigidBodyComponent
{
	float mass;
	float gravityScale;
	float jump;
	float dampening;
	myMath::Vector2D velocity;
	myMath::Vector2D acceleration;
	myMath::Vector2D force;

};