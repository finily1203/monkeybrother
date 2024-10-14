#include <GL/glew.h>    
#include "Engine.h"
#include "Systems.h"
#include "GlfwFunctions.h"
#include "GlobalCoordinator.h"
#include <iostream>

Engine::Engine() {}

void Engine::addSystem(GameSystems* system) {
	m_systems.push_back(system);
}

void Engine::initialiseSystem() {
	for (auto& system : m_systems) {
		system->initialise();
	}

	ecsCoordinator.initialise();
	
}

void Engine::updateSystem() {
	debugSystem.StartLoop();
	ecsCoordinator.update();

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
		default:
			break;
		}
			
	}
	debugSystem.EndLoop();
	debugSystem.UpdateSystemTimes();
}

void Engine::cleanupSystem() {
	for (auto& system : m_systems) {
		if (system != nullptr) {
			system->cleanup();
			
			if (system != &ecsCoordinator && system != &debugSystem) {
				delete system;
			}
		}
	}

	ecsCoordinator.cleanup();
	
}

Engine::~Engine() {}