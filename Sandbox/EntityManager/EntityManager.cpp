#include "EntityManager.h"
#include "ECSDefinitions.h"
#include <iostream>

EntityManager::EntityManager() {
	for (Entity ids = 0; ids < MAX_ENTITIES; ids++) {
		availableEnt.push(ids);
	}
}

Entity EntityManager::createEntity() {
	if (liveEntCount >= MAX_ENTITIES) { //checks if too much entities created
		std::cerr << "Max entities reached" << std::endl;
		return MAX_ENTITIES;
	}

	Entity entityId = availableEnt.front(); //get the entity id
	availableEnt.pop(); //pop the entity id
	liveEntCount++; //increment the live entity count

	std::cout << "Entity Created: " << entityId << std::endl;

	return entityId;
}

void EntityManager::destroyEntity(Entity entity) {
	if (entity >= MAX_ENTITIES || entity < 0) { //checks if entity is valid
		std::cerr << "Entity is not valid" << std::endl;
		return;
	}

	entitySig[entity].reset(); //destroy entity from entitySig
	availableEnt.push(entity); //push back the number to availableEnt
	liveEntCount--; //decrement the live entity count
}
void EntityManager::setSignature(Entity entity, ComponentSig signature) {
	if (entity >= MAX_ENTITIES || entity < 0) { //checks if entity is valid
		std::cerr << "Entity is not valid" << std::endl;
		return;
	}
	
	//do bitwise operation to set the signature
	entitySig[entity] |= signature;
}

void EntityManager::rmvSignature(Entity entity, ComponentSig signature) {
	if (entity >= MAX_ENTITIES || entity < 0) { //checks if entity is valid
		std::cerr << "Entity is not valid" << std::endl;
		return;
	}

	//do bitwise operation to set the signature
	entitySig[entity] &= ~signature;
}


ComponentSig EntityManager::getSignature(Entity entity) {
	if (entity >= MAX_ENTITIES || entity < 0) { //checks if entity is valid
		std::cerr << "Entity is not valid" << std::endl;
		return ComponentSig();
	}

	return entitySig[entity]; //return the signature of the entity
}

unsigned int EntityManager::getLiveEntCount() {
	return liveEntCount;
}

unsigned int EntityManager::getAvailableEntCount() {
	return availableEnt.size();
}

void EntityManager::cleanup() {
	availableEnt = std::queue<Entity>();
	liveEntCount = 0;
}