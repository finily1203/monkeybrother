/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   ComponentManager.cpp
@brief:  This source file defines the ComponentManager class for the ECS system.
		 The ComponentManager class is used to manage all the components in the ECS
		 system. It talks to all the ComponentHandlers to add, remove and retrieve
		 components from the entity. Some of the function definition can be found
		 here as functions template need to be defined in the header file.
		 Joel Chu (c.weiyuan): Define the functions in ComponentManager class mainly
							   to remove the entity and cleanup function for the
							   component handlers.
							   100%
*//*___________________________________________________________________________-*/
#include "ComponentManager.h"

//removes the entity from all component handlers
void ComponentManager::entityRemoved(Entity entity) {
	for (auto const& pair : componentHandlers) {
		auto const& component = pair.second;
		component->entityRemoved(entity);
	}
}

//cleanup all component handlers
void ComponentManager::cleanup() {
	componentHandlers.clear();
	componentTypes.clear();
	nextComponentType = 0;
}