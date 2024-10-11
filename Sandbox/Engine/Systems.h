/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   Systems.h
@brief:  This header file is declares the base class GameSystem for the game engine.
		 Current GameSystem class includes: ECS, Audio and Window.
		 Joel Chu (c.weiyuan): Declared the base class GameSystem.
							   100%
*//*___________________________________________________________________________-*/
#pragma once

//Systems used for the game
class GameSystems
{
public:
	//base functions to be inherited by other systems
	virtual void initialise() = 0;
	virtual void update() = 0;
	virtual void cleanup() = 0;
	virtual ~GameSystems() = default;
};