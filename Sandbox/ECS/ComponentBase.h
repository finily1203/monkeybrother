/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   ComponentBase.h
@brief:  This header file declares the base class ComponentBase for the ECS system.
		 This class is used to create a generic instance / Base form of a Component 
		 Class. The handlers will ensure that the components are added, removed and
		 retrieved from the entity.
		 Joel Chu (c.weiyuan): Declared the base class ComponentBase and 
							   ComponentHandler class.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "EntityManager.h"

#include <queue>
#include <array>
#include <bitset>
#include <string>
#include <unordered_map>
#include <string>
#include <cassert>
#include <iostream>
#include <set>

//Create a generic instance / Base form of a Component Class
//Every component class will inherit from this class
class ComponentBase
{
public:
	ComponentBase() = default;
	virtual ~ComponentBase() = default;
	virtual void entityRemoved(Entity entity) = 0;
};


//Manages components of type T of an entity
template <typename T>
class ComponentHandler : public ComponentBase
{
public:
	//adds component to entity
	void addComponentHandler(Entity entity, T component)
	{
		assert(entity < MAX_ENTITIES && entity >= 0 && "Entity is not valid!");
		//check if component already exists
		assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component already exists in the entity");

		//add component to array
		std::cout << nextComponentIndex << std::endl;
		size_t newIndex = nextComponentIndex;
		entityToIndexMap[entity] = newIndex;
		indexToEntityMap[newIndex] = entity;
		componentArray[newIndex] = component;
		nextComponentIndex++;
	}

	//removes component from entity
	void removeComponentHandler(Entity entity)
	{
		assert(entity < MAX_ENTITIES && entity >= 0 && "Entity is not valid!");

		// First check if entity exists in the map
		auto it = entityToIndexMap.find(entity);
		if (it == entityToIndexMap.end()) {
			return; // Entity doesn't have this component, nothing to remove
		}

		size_t removedIndex = it->second;
		// Check if the index is valid
		if (removedIndex >= MAX_ENTITIES) {
			return; // Invalid index, prevent array out of bounds
		}

		// Move last component to fill the gap
		size_t lastIndex = nextComponentIndex - 1;
		if (removedIndex != lastIndex && lastIndex < MAX_ENTITIES) {
			// Copy the last component to the removed slot
			componentArray[removedIndex] = componentArray[lastIndex];

			// Update the mappings for the moved component
			Entity lastEntity = indexToEntityMap[lastIndex];
			entityToIndexMap[lastEntity] = removedIndex;
			indexToEntityMap[removedIndex] = lastEntity;

			// Clear the last slot
			componentArray[lastIndex] = T{};
		}
		else {
			// If we're removing the last component or there's only one
			componentArray[removedIndex] = T{};
		}

		entityToIndexMap.erase(entity);
		indexToEntityMap.erase(removedIndex);
		if (nextComponentIndex > 0) {
			nextComponentIndex--;
		}
	}

	//retrieves the specific component struct / class type from entity
	T& getComponentHandler(Entity entity)
	{
		assert(entity < MAX_ENTITIES && entity >= 0 && "Entity is not valid!");
		return componentArray[entityToIndexMap[entity]];
	}

	//Checks if the entity has the component
	bool hasComponentHandler(Entity entity)
	{
		assert(entity < MAX_ENTITIES && entity >= 0 && "Entity is not valid!");
		return entityToIndexMap.find(entity) != entityToIndexMap.end();
	}

	//follow base class
	void entityRemoved(Entity entity)
	{
		if (entityToIndexMap.find(entity) != entityToIndexMap.end())
		{
			removeComponentHandler(entity);
		}
	}

private:
	//array of components of type T for the specific entity
	std::array<T, MAX_ENTITIES> componentArray;
	std::unordered_map<Entity, size_t> entityToIndexMap;
	std::unordered_map<size_t, Entity> indexToEntityMap;
	size_t nextComponentIndex = 0;
};
