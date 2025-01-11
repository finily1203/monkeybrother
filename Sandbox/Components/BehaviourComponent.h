/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   BehaviourComponent.h
@brief:  This header file includes a multitude of bool variables used by ECS to
 determine which entity has which specific behaviour. Note that the entity
 can only have one behaviour at a time despite the multiple bool variables.

 Joel Chu (c.weiyuan): declared the struct component
					   100%
*//*___________________________________________________________________________-*/

#pragma once
struct BehaviourComponent
{
	bool none;
	bool player;
	bool enemy;
	bool pump;
	bool exit;
	bool collectable;
	bool button;
	bool platform;

	BehaviourComponent() : none(false), player(false), enemy(false), pump(false), exit(false), collectable(false), button(false), platform(false) {}
};