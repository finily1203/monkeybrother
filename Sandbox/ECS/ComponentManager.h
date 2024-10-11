/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   ComponentManager.h
@brief:  This header file declares the ComponentManager class for the ECS system.
		 The ComponentManager class is used to manage all the components in the ECS
		 system. It talks to all the ComponentHandlers to add, remove and retrieve
		 components from the entity. Some of the function definition can be found
		 here as functions template need to be defined in the header file.
		 Joel Chu (c.weiyuan): Declared the ComponentManager class and its functions.
							   Some of the function definitions can be found in here
							   as well.
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "ComponentBase.h"
#include "EntityManager.h"

#include <queue>
#include <array>
#include <bitset>
#include <string>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <set>

//Purpose of ComponentManager is to talk to all the ComponentHandlers -> ComponentBase.h
class ComponentManager
{
public:
	//Register component handler
	template <typename T>
	void registerComponentHandler();

	//Add component to entity
	template <typename T>
	void addComponent(Entity entity, T component);

	//Remove component from entity
	template <typename T>
	void removeComponent(Entity entity);

	//Get component from entity by reference
	template <typename T>
	T& getComponent(Entity entity);

	////Get component struct / class to access values inside it
	template <typename T>
	ComponentType getComponentType();

	//Helper function
	template <typename T>
	std::shared_ptr<ComponentHandler<T>> getComponentHandler();

	//for clone entity
	template <typename T>
	void cloneComponent(Entity entity, Entity newEntity);

	//for checking if entity has component
	template <typename T>
	bool hasComponent(Entity entity);

	//Removes entity from all components
	void entityRemoved(Entity entity);

	//Cleanup function
	void cleanup();

private:
	std::map<const std::string, ComponentType> componentTypes;
	ComponentType nextComponentType = 0;

	std::map<const std::string, std::shared_ptr<ComponentBase>> componentHandlers;

};

//Register component handler
template <typename T>
void ComponentManager::registerComponentHandler() {
	//check if component type already exists
	const std::string typeName = typeid(T).name();
	assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once.");

	//add component type to the component types map
	componentTypes.insert({ typeName, nextComponentType });
	componentHandlers.insert({ typeName, std::make_shared<ComponentHandler<T>>() });
	nextComponentType++;
}

//Adds component to entity
template <typename T>
void ComponentManager::addComponent(Entity entity, T component) {
	getComponentHandler<T>()->addComponentHandler(entity, component);
}

//Removes component from entity
template <typename T>
void ComponentManager::removeComponent(Entity entity) {
	getComponentHandler<T>()->removeComponentHandler(entity);
}

//Get component from entity by reference
template <typename T>
T& ComponentManager::getComponent(Entity entity) {
	return getComponentHandler<T>()->getComponentHandler(entity);	
}


//Get component struct / class to access values inside it
template <typename T>
ComponentType ComponentManager::getComponentType() {
	const std::string typeName = typeid(T).name();
	assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");

	return componentTypes[typeName];
}

//Helper function
template <typename T>
std::shared_ptr<ComponentHandler<T>> ComponentManager::getComponentHandler() {
	const std::string typeName = typeid(T).name();
	assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");

	return std::dynamic_pointer_cast<ComponentHandler<T>>(componentHandlers[typeName]);
}

//To clone entity
template <typename T>
void ComponentManager::cloneComponent(Entity entity, Entity newEntity) {
	T component = getComponent<T>(entity);
	addComponent<T>(newEntity, component);
}

//Checks if entity has the component given
template <typename T>
bool ComponentManager::hasComponent(Entity entity) {
	return getComponentHandler<T>()->hasComponentHandler(entity);
}