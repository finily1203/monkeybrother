#pragma once
#include "vector2D.h"

struct RigidBodyComponent
{
	float speed;
	float mass;
	float gravityScale;
	myMath::Vector2D velocity;
	myMath::Vector2D acceleration;
	myMath::Vector2D force;

};