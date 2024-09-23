#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"

#include <queue>
#include <array>
#include <bitset>
#include <string>
#include <unordered_map>
#include <string>
#include <cassert>
#include <iostream>
#include <set>

class System {
public:
	std::set<Entity> entities;
};

class SystemManager
{
public:
	//Register system into system manager
	template <typename T>
	std::shared_ptr<T> registerSystem();

	//Set the signature of the system
	template <typename T>
	void setSystemSignature(ComponentSig signature);

	//Entity manager will call this function
	void entityRemoved(Entity entity);

	//Change the signature of the entity
	void entitySigChange(Entity entity, ComponentSig entitySig);

private:
	std::unordered_map<std::string, ComponentSig> systemSignatures;
	std::unordered_map<std::string, std::shared_ptr<System>> Systems;
};