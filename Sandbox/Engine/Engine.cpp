/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   GLFWFunctions.cpp
@brief:  This source file includes the functions to handle gameSystem class. From
		 initialising, updating and cleaning up the systems.
		 Joel Chu (c.weiyuan): Defined functions to add, initialise, update and cleanup
							   systems.
							   100%
*//*___________________________________________________________________________-*/

#include <GL/glew.h>    
#include "Engine.h"
#include "Systems.h"
#include "GlfwFunctions.h"
#include "GlobalCoordinator.h"
#include <iostream>

Engine::Engine() {}

//Add GameSystem type to the engine
void Engine::addSystem(GameSystems* system) {
	m_systems.push_back(system);
}

//Initialise all added systems. Should be used after all systems are added
void Engine::initialiseSystem() {
	for (auto& system : m_systems) {
		system->initialise();
	}

	ecsCoordinator.initialise();
}

//Update all systems. Should be used after all systems are initialised
void Engine::updateSystem() {
	ecsCoordinator.update();

	for (auto& system : m_systems) {
		system->update();
	}
}

//Clean up all systems. Should be used after all systems are updated
void Engine::cleanupSystem() {
	for (auto& system : m_systems) {
		if (system != nullptr) {
			system->cleanup();
			
			if (system != &ecsCoordinator && system != &fontSystem) {
				delete system;
			}
		}
	}

	ecsCoordinator.cleanup();
}

Engine::~Engine() {}