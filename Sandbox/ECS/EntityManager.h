/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EntityManager.h
@brief:  This header file declares the EntityManager class for the ECS system.
		 The EntityManager class is used to manage all the entities in the ECS.
		 Joel Chu (c.weiyuan): declared the functions in EntityManager class
							   100%
*//*___________________________________________________________________________-*/
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
	unsigned int getLiveEntCount();
	unsigned int getAvailableEntCount();

	std::vector<Entity> getLiveEntities();

	Entity getEntityById(std::string const& id);
	void setEntityId(Entity entity, std::string const& id);
	std::string getEntityId(Entity entity);
	Entity getEntityFromId(std::string const& id);

	void setTextureId(Entity entity, std::string const& id);

	std::unordered_map<Entity, std::string> getEntityMap() const;

	void cleanup();

private:
	//thinking of the ID as a last in first out kind of thing
	std::queue<Entity> availableEnt; //use queue to add and remove entities
	std::array<ComponentSig, MAX_ENTITIES> entitySig; 
	std::unordered_map<Entity, std::string> entityIds;
	std::unordered_map<Entity, std::string> textureIds;
	unsigned int liveEntCount = 0; 
};