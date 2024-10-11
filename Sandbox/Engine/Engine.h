/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   Engine.h
@brief:  This header file includes the class Engine that handles all the gameSystems
         that will be used by the GameEngine. Handles the lives of gameSystems from
		 start to end.
		 Joel Chu (c.weiyuan): Declared the class Engine and its functions.
							   100%
*//*___________________________________________________________________________-*/
#pragma once

#include <vector>
#include "Systems.h"

//Engine for the entire game
class Engine
{
private:
	//Using vectors to store systems
	std::vector<GameSystems*> m_systems;
public:
	//public functions used by engine
	Engine();

	//Add GameSystem type to the engine
	void addSystem(GameSystems* systems);

	//Initialise all added systems. Should be used after all systems are added
	void initialiseSystem();
	//Update all systems. Should be used after all systems are initialised
	void updateSystem();
	//Clean up all systems. Should be used after all systems are updated
	void cleanupSystem();

	~Engine();
};
