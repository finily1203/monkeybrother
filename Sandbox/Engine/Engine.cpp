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
	for (auto& system : m_systems) {
		system->update();
	}

	ecsCoordinator.update();
}

void Engine::cleanupSystem() {
	for (auto& system : m_systems) {
		if (system != nullptr) {
			system->cleanup();
			
			if (system != &ecsCoordinator) {
				delete system;
			}
		}
	}

	ecsCoordinator.cleanup();
}

Engine::~Engine() {}