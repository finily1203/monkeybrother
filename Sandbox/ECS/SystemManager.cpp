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
#include "EngineDefinitions.h"
#include <GL/glew.h>  // GLEW must come first
#include <GL/gl.h>    // Then regular OpenGL
#include <GLFW/glfw3.h>  // Then GLFW
#include "SystemManager.h"
#include "GlobalCoordinator.h"
#include "GUIGameViewport.h"
#include "WindowSystem.h"

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
		if (GameViewWindow::getPaused() || WindowSystem::GetAltTab() || WindowSystem::GetCtrlAltDel() ) {
			if (system->getSystemECS() == "LogicSystemECS" || system->getSystemECS() == "PhysicsColliSystemECS") {
				continue;
			}
		}
		if (system->getSystemECS() == "PhysicsColliSystemECS") {
			debugSystem.StartSystemTiming("PhysicsColliSystemECS");
		}
		if (system->getSystemECS() == "GraphicsSystemECS") {
			debugSystem.StartSystemTiming("GraphicsSystemECS");
		}
		if (system->getSystemECS() == "LogicSystemECS") {
			debugSystem.StartSystemTiming("LogicSystemECS");
		}
		if (system->getSystemECS() == "FontSystemECS") {
			debugSystem.StartSystemTiming("FontSystemECS");
		}
		
		
		system->update(GLFWFunctions::delta_time);

		if (system->getSystemECS() == "PhysicsColliSystemECS") {
			debugSystem.EndSystemTiming("PhysicsColliSystemECS");
		}
		if (system->getSystemECS() == "GraphicsSystemECS") {
			debugSystem.EndSystemTiming("GraphicsSystemECS");
		}
		if (system->getSystemECS() == "LogicSystemECS") {
			debugSystem.EndSystemTiming("LogicSystemECS");
		}
		if (system->getSystemECS() == "FontSystemECS") {
			debugSystem.EndSystemTiming("FontSystemECS");
		}
		
	}
}

void SystemManager::cleanup() {
	Systems.clear();
	std::unordered_map<std::string, std::shared_ptr<System>>().swap(Systems);
	systemSignatures.clear();
	std::unordered_map<std::string, ComponentSig>().swap(systemSignatures);
}