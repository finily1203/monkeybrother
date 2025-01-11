/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Lew Zong Han Owen (z.lew)
@team:   MonkeHood
@course: CSD2401
@file:   ExitComponent.h
@brief:  This header file includes a bool variable used by ECS to ensure that
 graphics system knows which entity to render as a exit. This is
 also used as part of the exit behaviour within the logicSystemECS.

 Lew Zong Han Owen (z.lew): declared the struct component
							100%
*//*___________________________________________________________________________-*/
#pragma once
struct ExitComponent
{
	bool isExit;

	ExitComponent() : isExit(false) {}
};