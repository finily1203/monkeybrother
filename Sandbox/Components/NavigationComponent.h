/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Liu Yaoting (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   NavigationComponent.h
@brief:  This header file includes the implementation of the Navigation Component
		 for the game to handle navigation of collectables as well as the exit.

		 Liu Yaoting (yaoting.loi): declared the struct component
								    100%
*//*___________________________________________________________________________-*/
#pragma once

struct NavigationComponent
{
	bool isNavigation;
	bool isVisible;

	NavigationComponent() : isNavigation(false), isVisible(false) {}
};