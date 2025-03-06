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