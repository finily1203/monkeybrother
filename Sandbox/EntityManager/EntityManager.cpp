#include "EntityManager.h"
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

void EntityManager::testEntityManager() {
	std::cout << "Create Entity Test" << std::endl;

	Entity ent1 = createEntity();
	Entity ent2 = createEntity();
	Entity ent3 = createEntity();

	std::cout << "Entity 1: " << ent1 << std::endl;
	std::cout << "Entity 2: " << ent2 << std::endl;
	std::cout << "Entity 3: " << ent3 << std::endl;

	std::cout << "Set Signature Test" << std::endl;
	ComponentSig physicsSig;
	ComponentSig renderSig;
	ComponentSig inputSig;
	physicsSig.set(0);
	renderSig.set(1);
	inputSig.set(2);
	setSignature(ent1, physicsSig);
	setSignature(ent2, renderSig);
	setSignature(ent3, inputSig);

	std::cout << "1st Entity contains: " << getSignature(ent1)  << std::endl; //physicsSig
	std::cout << "2nd Entity contains: " << getSignature(ent2)  << std::endl; //renderSig
	std::cout << "3rd Entity contains: " << getSignature(ent3)  << std::endl; //inputSig
	std::cout << "Entities Left: "		 << liveEntCount	    << std::endl; //3
	std::cout << "Available Entities: "  << availableEnt.size() << std::endl; //4997


	std::cout << "Destroy Entity Test" << std::endl;
	destroyEntity(ent1);

	std::cout << "1st Entity contains: " << getSignature(ent1)  << std::endl; //empty
	std::cout << "2nd Entity contains: " << getSignature(ent2)  << std::endl; //renderSig
	std::cout << "3rd Entity contains: " << getSignature(ent3)  << std::endl; //inputSig
	std::cout << "Entities Left: "		 << liveEntCount	    << std::endl; //2
	std::cout << "Available Entities: "  << availableEnt.size() << std::endl; //4997
}


//SET UP A WAY FOR COMPONENTS NAMES TO BE TIED TO COMPONENT SIGNATURES//