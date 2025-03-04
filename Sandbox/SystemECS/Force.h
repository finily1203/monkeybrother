/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Lee Jing Wen (jingwen.lee)
@team:   MonkeHood
@course: CSD2401
@file:   Force.h
@brief:  This source file ccontains the definition of the Force and ForceManager
		 classes to be used by Physics and Collision Systems.
		 Lee Jing Wen (jingwen.lee): Declared the Force and ForceManager classes
									 100%
*//*____________________________________________________________________________-*/
#pragma once
#include "vector2D.h"

class Force
{
public:

	Force(myMath::Vector2D direction, float magnitude) : direction(direction), magnitude(magnitude) {}

	myMath::Vector2D GetDirection() const { return direction; }
	void SetDirection(myMath::Vector2D newDirection) { direction = newDirection; }

	float GetMagnitude() const { return magnitude; }
	void SetMagnitude(float newMagnitude) { magnitude = newMagnitude; }

private:
	myMath::Vector2D direction;
	float magnitude;
};

class ForceManager
{
public:

	void AddForce(Entity player, const myMath::Vector2D& appliedForce);

	void ClearForce(Entity player);

	void ApplyForce(Entity player, myMath::Vector2D direction, float magnitude);

	float ResultantForce(myMath::Vector2D direction, myMath::Vector2D normal, float maxAccForce);
};