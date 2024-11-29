/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Ian Loi (ian.loi)
@team:   MonkeHood
@course: CSD2401
@file:   ButtonComponent.h
@brief:  This header file includes the ButtonComponent used by ECS to handle mouse
		 on-click and on-hover 
		 Ian Loi (ian.loi): Declared the ButtonComponent struct
							100%
*//*___________________________________________________________________________-*/
#pragma once
#include "vector2D.h"

struct ButtonComponent
{
	myMath::Vector2D originalScale;
	myMath::Vector2D hoveredScale;
	bool isButton;

	ButtonComponent() : originalScale({ 0.f, 0.f }), hoveredScale({ 0.f, 0.f }), isButton(false) {}
};
