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
	template <typename T>
	void registerComponentHandler();

	template <typename T>
	void addComponent(Entity entity, T component);

	template <typename T>
	void removeComponent(Entity entity);

	template <typename T>
	T& getComponent(Entity entity);

	template <typename T>
	ComponentType getComponentType();

	//Helper function
	template <typename T>
	std::shared_ptr<ComponentHandler<T>> getComponentHandler();

	//for clone entity
	template <typename T>
	void cloneComponent(Entity entity, Entity newEntity);


	void entityRemoved(Entity entity);

	void cleanup();

private:
	//Mark down all the component types by linking them to a const char*
	std::map<const std::string, ComponentType> componentTypes;
	ComponentType nextComponentType = 0;

	//using a smart pointer to store the component handler
	//using shared_ptr instead of unique_ptr since unique_ptr can only point to one but shared can point to multiple
	std::map<const std::string, std::shared_ptr<ComponentBase>> componentHandlers;

};

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

template <typename T>
void ComponentManager::addComponent(Entity entity, T component) {
	//auto* componentHandler = getComponentHandler<T>().get();
	//componentHandler->addComponentHandler(entity, component);
	getComponentHandler<T>()->addComponentHandler(entity, component);
}

template <typename T>
void ComponentManager::removeComponent(Entity entity) {
	//auto* componentHandler = getComponentHandler<T>();
	//componentHandler->removeComponentHandler(entity);
	getComponentHandler<T>()->removeComponentHandler(entity);
}

template <typename T>
T& ComponentManager::getComponent(Entity entity) {
	//auto* componentHandler = getComponentHandler<T>();
	//componentHandler->getComponentHandler(entity);
	return getComponentHandler<T>()->getComponentHandler(entity);	
}

template <typename T>
ComponentType ComponentManager::getComponentType() {
	const std::string typeName = typeid(T).name();
	assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");

	return componentTypes[typeName];
}

template <typename T>
std::shared_ptr<ComponentHandler<T>> ComponentManager::getComponentHandler() {
	const std::string typeName = typeid(T).name();
	assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered");

	return std::dynamic_pointer_cast<ComponentHandler<T>>(componentHandlers[typeName]);
}

template <typename T>
void ComponentManager::cloneComponent(Entity entity, Entity newEntity) {
	T component = getComponent<T>(entity);
	addComponent<T>(newEntity, component);
}

void ComponentManager::entityRemoved(Entity entity) {
	for (auto const& pair : componentHandlers) {
		auto const& component = pair.second;
		component->entityRemoved(entity);
	}
}

void ComponentManager::cleanup() {
	componentHandlers.clear();
	componentTypes.clear();
	nextComponentType = 0;
}