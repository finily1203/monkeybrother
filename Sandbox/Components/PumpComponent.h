/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   PumpComponent.h
@brief:  This header file includes a bool variable used by ECS to ensure that
 graphics system knows which entity to render as a pump. This is
 also used as part of the pumpEffect behaviour within the logicSystemECS.

 Joel Chu (c.weiyuan): declared the struct component
					   100%
*//*___________________________________________________________________________-*/

#pragma once
struct PumpComponent
{
	bool isPump;
	bool isAnimate;
	float pumpForce;
	PumpComponent() : isPump(false), isAnimate(false), pumpForce(0.f) {}
};