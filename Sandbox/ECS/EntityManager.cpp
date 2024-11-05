/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EntityManager.cpp
@brief:  This source file defines the EntityManager class for the ECS system.
		 The EntityManager class is used to manage all the entities in the ECS.
		 Joel Chu (c.weiyuan): Defined the functions in EntityManager class
							   100%
*//*___________________________________________________________________________-*/

#include "EntityManager.h"
#include "ECSDefinitions.h"
#include <iostream>

//Constructor to create entities
EntityManager::EntityManager() {
	for (Entity ids = 0; ids < MAX_ENTITIES; ids++) {
		availableEnt.push(ids);
	}
}


//Create an entity
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
	entityIds.erase(entity); //erase the entity id
	entitySig[entity].reset(); //destroy entity from entitySig
	availableEnt.push(entity); //push back the number to availableEnt

	liveEntCount--; //decrement the live entity count
}

//Set siganature of entity
void EntityManager::setSignature(Entity entity, ComponentSig signature) {
	if (entity >= MAX_ENTITIES || entity < 0) { //checks if entity is valid
		std::cerr << "Entity is not valid" << std::endl;
		return;
	}
	
	//do bitwise operation to set the signature
	entitySig[entity] |= signature;
}


//Remove signatures from entity
void EntityManager::rmvSignature(Entity entity, ComponentSig signature) {
	if (entity >= MAX_ENTITIES || entity < 0) { //checks if entity is valid
		std::cerr << "Entity is not valid" << std::endl;
		return;
	}

	//do bitwise operation to set the signature
	entitySig[entity] &= ~signature;
}

//Retrieve signature from entity
ComponentSig EntityManager::getSignature(Entity entity) {
	if (entity >= MAX_ENTITIES || entity < 0) { //checks if entity is valid
		std::cerr << "Entity is not valid" << std::endl;
		return ComponentSig();
	}

	return entitySig[entity]; //return the signature of the entity
}


//Retrieve the number of live entities
unsigned int EntityManager::getLiveEntCount() {
	return liveEntCount;
}


//Retrieve the number of available entities
unsigned int EntityManager::getAvailableEntCount() {
	return static_cast<unsigned int>(availableEnt.size());
}

// Retrieve the entity object based on the entity Id
Entity EntityManager::getEntityById(std::string const& id)
{
	for (const auto& pair : entityIds)
	{
		if (pair.second == id)
		{
			return pair.first;
		}
	}

	return 0;
}

// Set the entity Id based on the entity object
void EntityManager::setEntityId(Entity entity, std::string const& id)
{
	entityIds[entity] = id;
}

// retrieve the entity Id
std::string EntityManager::getEntityId(Entity entity)
{
	for (const auto& pair : entityIds)
	{
		if (pair.first == entity)
		{
			return pair.second;
		}
	}

	return "";
}

Entity EntityManager::getEntityFromId(std::string const& id)
{
	Entity entity = 0;
	for (const auto& pair : entityIds)
	{
		if (pair.second == id) {
			return pair.first;
		}
	}
	return entity;
}

// Retrieve the entity map
std::unordered_map<Entity, std::string> EntityManager::getEntityMap() const
{
	return entityIds;
}

//Cleanup the entity manager by resetting the available entities and live entity count
void EntityManager::cleanup() {
	availableEnt = std::queue<Entity>();
	liveEntCount = 0;
}


//Retrieve all the live entities
std::vector<Entity> EntityManager::getLiveEntities() {
	std::vector<Entity> liveEntities;
	for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
		if (entitySig[entity].any()) {
			liveEntities.push_back(entity);
		}
	}
	return liveEntities;
}