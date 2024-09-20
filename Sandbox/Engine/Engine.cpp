#include "Engine.h"

Engine::Engine() {}

void Engine::addSystem(Systems* system) {
	systems.push_back(system);
}

void Engine::initialiseSystem() {
	for (auto& system : systems) {
		system->initialise();
	}
}

void Engine::updateSystem() {
	for (auto& system : systems) {
		system->update();
	}
}

void Engine::cleanupSystem() {
	for (auto& system : systems) {
		system->cleanup();
		delete system;
	}
}

Engine::~Engine() {}