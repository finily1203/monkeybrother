/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   CollectableComponent.h
@brief:  This header file includes a bool variable used by ECS to ensure that
 graphics system knows which entity to render as a collectable. This is
 also used as part of the collectable behaviour within the logicSystemECS.

 Joel Chu (c.weiyuan): declared the struct component
					   100%
*//*___________________________________________________________________________-*/

#pragma once
struct CollectableComponent
{
	bool isCollectable;

	CollectableComponent() : isCollectable(false) {}
};