/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   AABBComponent.h
@brief:  This header file includes the AABB Component used by ECS to handle
		 and PhysicsCollisionSystem to handle collision detection.
		 Joel Chu (c.weiyuan): declared the struct component
							   100%
*//*___________________________________________________________________________-*/
#pragma once
//left, right, top, bottom
struct AABBComponent 
{
	float left, right, top, bottom;
};