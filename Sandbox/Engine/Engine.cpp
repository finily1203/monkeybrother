#include "Engine.h"
#include "Systems.h"
#include "GlfwFunctions.h"

Engine::Engine() {}

void Engine::addSystem(GameSystems* system) {
	m_systems.push_back(system);
}

void Engine::initialiseSystem() {
	for (auto& system : m_systems) {
		system->initialise();
	}
}

void Engine::updateSystem() {
	for (auto& system : m_systems) {
		system->update();
	}
}

void Engine::cleanupSystem() {
	for (auto& system : m_systems) {
		system->cleanup();
		delete system;
	}
}

Engine::~Engine() {}