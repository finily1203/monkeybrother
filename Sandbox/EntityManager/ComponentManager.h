#pragma once
#include "EntityManager.h" //Remove later
#include "ComponentBase.h"
#include <unordered_map>
#include <string>
#include <cassert>
#include <iostream>

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;

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
	ComponentHandler<T>* getComponentHandler();


	void entityRemoved(Entity entity);

private:
	//Mark down all the component types by linking them to a const char*
	std::unordered_map<std::string, ComponentType> componentTypes;
	ComponentType nextComponentType = 0;

	//using a smart pointer to store the component handler
	//using shared_ptr instead of unique_ptr since unique_ptr can only point to one but shared can point to multiple
	std::unordered_map<std::string, std::shared_ptr<ComponentBase>> componentHandlers;

};