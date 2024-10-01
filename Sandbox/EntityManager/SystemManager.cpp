#include "SystemManager.h"

void SystemManager::entityRemoved(Entity entity) {
	//erase entity from all systems
	for (auto const& pair : Systems) {
		auto const& system = pair.second;
		system->entities.erase(entity);
	}
}

void SystemManager::entitySigChange(Entity entity, ComponentSig entitySig) {
	//update entity signature in all systems
	for (auto const& pair : Systems) {
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSig = systemSignatures[type];

		std::cout << "Checking entity " << entity << " with signature: " << entitySig << std::endl;
		std::cout << "System signature: " << systemSig << std::endl;

		//check if entity signature matches system signature
		if ((entitySig & systemSig) == systemSig) {
			system->entities.insert(entity);
		}
		else {
			system->entities.erase(entity);
		}
	}
}

void SystemManager::update() {
	for (auto const& pair : Systems) {
		auto const& system = pair.second;
		system->update(GLFWFunctions::delta_time);
	}
}

void SystemManager::cleanup() {
	Systems.clear();
	systemSignatures.clear();
}