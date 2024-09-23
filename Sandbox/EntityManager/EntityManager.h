//Purpose: To manage entities and their components

#pragma once
#include <queue>
#include <array>
#include <bitset>
#include <string>
#include <unordered_map>
#include <string>
#include <cassert>
#include <iostream>
#include <set>
#include "ECSDefinitions.h"

class EntityManager
{
public:
	EntityManager();
	Entity createEntity();
	void destroyEntity(Entity entity);
	void setSignature(Entity entity, ComponentSig signature);
	void rmvSignature(Entity entity, ComponentSig signature);
	ComponentSig getSignature(Entity entity);
	void testEntityManager();
	unsigned int getLiveEntCount();
	unsigned int getAvailableEntCount();

private:
	//thinking of the ID as a last in first out kind of thing
	std::queue<Entity> availableEnt; //use queue to add and remove entities
	std::array<ComponentSig, MAX_ENTITIES> entitySig; 
	unsigned int liveEntCount = 0; 
};