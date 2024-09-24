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

template <typename T>
std::shared_ptr<T> SystemManager::registerSystem() {
	std::string typeName = typeid(T).name();
	//checks if system already exists
	assert(Systems.find(typeName) == Systems.end() && "Registering system more than once.");
	auto system = std::make_shared<T>();
	Systems.insert({ typeName, system });
	return system;
}

template <typename T>
void SystemManager::setSystemSignature(ComponentSig signature) {
	std::string typeName = typeid(T).name();
	//checks if system already exists
	assert(Systems.find(typeName) != Systems.end() && "System used before registered.");
	systemSignatures.insert({ typeName, signature });
}

void SystemManager::entityRemoved(Entity entity) {
	//erase entity from all systems
	for (auto const& pair : Systems) {
		auto const& system = pair.second;
		system->entities.erase(entity);
	}
}

void SystemManager::entitySigChange(Entity entity, ComponentSig entitySig) {
	//update entity signature in all systems
	for (auto const& pair : Systems) {
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSig = systemSignatures[type];

		//check if entity signature matches system signature
		if ((entitySig & systemSig) == systemSig) {
			system->entities.insert(entity);
		}
		else {
			system->entities.erase(entity);
		}
	}
}