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
#include "EngineDefinitions.h"
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
	debugSystem.StartLoop();
	//ecsCoordinator.update();

	for (auto& system : m_systems) {

		//Start time record for perfomance viewer
		switch (system->getSystem()) {
		case SystemType::AudioSystemType:
			debugSystem.StartSystemTiming("AudioSystem");
			break;
		case SystemType::WindowSystemType:
			debugSystem.StartSystemTiming("WindowSystem");
			break;
		case SystemType::DebugSystemType:
			debugSystem.StartSystemTiming("DebugSystem");
			break;
		case SystemType::ECSType:
			debugSystem.StartSystemTiming("EntityComponentSystem");
			break;
		case SystemType::AssetsManagerType:
			debugSystem.StartSystemTiming("AssetsManager");
			break;
		default:
			break;
		}
		
		system->update();
		
		//End time record for perfomance viewer
		switch (system->getSystem()) {
		case SystemType::AudioSystemType:
			debugSystem.EndSystemTiming("AudioSystem");
			break;
		case SystemType::WindowSystemType:
			debugSystem.EndSystemTiming("WindowSystem");
			break;
		case SystemType::DebugSystemType:
			debugSystem.EndSystemTiming("DebugSystem");
			break;
		case SystemType::ECSType:
			debugSystem.EndSystemTiming("EntityComponentSystem");
			break;
		case SystemType::AssetsManagerType:
			debugSystem.EndSystemTiming("AssetsManager");
			break;
		default:
			break;
		}
			
	}
	debugSystem.EndLoop();
	debugSystem.UpdateSystemTimes();
}

//Clean up all systems. Should be used after all systems are updated
void Engine::cleanupSystem() {
	for (auto& system : m_systems) {
		if (system != nullptr) {
			system->cleanup();
			
			if (system != &ecsCoordinator && system != &debugSystem && system != &fontSystem 
				&& system != &assetsManager && system != &graphicsSystem) {
				delete system;
			}
		}
	}

	ecsCoordinator.cleanup();
	
}

Engine::~Engine() {}