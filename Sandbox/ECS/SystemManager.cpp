/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   SystemManager.cpp
@brief:  This source file defines the SystemManager class for the ECS system.
		 The SystemManager class is used to manage all the systems within the ECS.
		 Joel Chu (c.weiyuan): Defined the functions in SystemManager class
							   100%
*//*___________________________________________________________________________-*/
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