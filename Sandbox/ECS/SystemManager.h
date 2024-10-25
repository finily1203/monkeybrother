/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   SystemManager.h
@brief:  This header file declares the functions the SystemManager class for the 
		 ECS system. The SystemManager class is used to manage all the systems 
		 within the ECS.
		 Joel Chu (c.weiyuan): Declared the functions in SystemManager class
							   and some function templates.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "EntityManager.h"
#include "GLFWFunctions.h"

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
	virtual ~System() = default;
	
	virtual void initialise() = 0;
	virtual void update(float dt) = 0;
	virtual void cleanup() = 0;
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

	void update();

	void cleanup();

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