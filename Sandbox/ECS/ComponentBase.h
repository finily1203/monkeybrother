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
	void addComponentHandler(Entity entity, T component)
	{
		assert(entity < MAX_ENTITIES && entity >= 0 && "Entity is not valid!");
		//check if component already exists
		assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component already exists in the entity");

		//add component to array
		size_t newIndex = nextComponentIndex;
		entityToIndexMap[entity] = newIndex;
		indexToEntityMap[newIndex] = entity;
		componentArray[newIndex] = component;
		nextComponentIndex++;
	}

	void removeComponentHandler(Entity entity)
	{
		assert(entity < MAX_ENTITIES && entity >= 0 && "Entity is not valid!");

		auto it = entityToIndexMap.find(entity);
		assert(it != entityToIndexMap.end() && "Component does not exist in the entity");
		size_t removedIndex = it->second;
		componentArray[removedIndex] = T{};
		entityToIndexMap.erase(entity);
		indexToEntityMap.erase(removedIndex);
		nextComponentIndex--;
	}

	T& getComponentHandler(Entity entity)
	{
		assert(entity < MAX_ENTITIES && entity >= 0 && "Entity is not valid!");
		return componentArray[entityToIndexMap[entity]];
	}

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