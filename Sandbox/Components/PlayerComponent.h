/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   PlayerComponent.h
@brief:  This header file includes a bool variable used by ECS to ensure that
 graphics system knows which entity to render as a player. This is also used
 in many other game object systems to handle logic of players and other
 entities.
 Joel Chu (c.weiyuan): declared the struct component
					   100%
*//*___________________________________________________________________________-*/

#pragma once
struct PlayerComponent
{
	bool isPlayer;
	bool isVisible;

	PlayerComponent() : isPlayer(false), isVisible(true) {}
};