//Purpose: To manage entities and their components

#pragma once
#include <queue>
#include <array>
#include <bitset>
#include <string>

using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 5000; // for now set to 5000 objects
using ComponentSig = std::bitset<32>; //Sig for Signature // for now set to 32 components

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
	std::queue<Entity> availableEnt{}; //use queue to add and remove entities
	std::array<ComponentSig, MAX_ENTITIES> entitySig; 
	unsigned int liveEntCount = 0; 
};