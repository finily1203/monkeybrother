/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan), Ian Loi (ian.loi)
@team:   MonkeHood
@course: CSD2401
@file:   ECSCoordinator.h
@brief:  This header file includes the class ECSCoordinator that handles all the ECS
		 parts of ECS and ensure that they are all able to communicate with each other.
		 Function templates can be found here instead of the source file.
		 Joel Chu (c.weiyuan): Declared the ECSCoordinator class that links the ECS
							   parts together. Declared most of the functions here
							   as well.
							   90%
		Ian Loi (ian.loi): Declared the LoadEntityFromJSON, SaveEntityToJSON and 
						   UpdateEntity functions that load, save and update data for
						   entities.
						   10%
*//*___________________________________________________________________________-*/
#pragma once
#include "EngineDefinitions.h"
#include <GL/glew.h>  // Include GLEW before OpenGL

#include <GL/gl.h>    // OpenGL header (after GLEW)
#include <GLFW/glfw3.h>  // GLFW (if you're using it)

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "ECSDefinitions.h"
#include "GlfwFunctions.h"
#include "GraphicsSystem.h"
#include "Systems.h"

#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "ClosestPlatform.h"
#include "GraphicsSystem.h"
#include "AnimationComponent.h"
#include "EnemyComponent.h"
#include "PhysicsComponent.h"
#include "FontComponent.h"
#include "PlayerComponent.h"
#include "ButtonComponent.h"
#include "CollectableComponent.h"
#include "PumpComponent.h"
#include "ExitComponent.h"

#include <iostream>
#include <fstream>
#include <vector>
#include "vector2D.h"
#include "matrix3x3.h"
#include "vector3D.h"
#include "../Serialization/jsonSerialization.h"
#include "../Serialization/serialization.h"
#include "../MessageSystem/baseMessageSystem.h"
#include "../MessageSystem/observable.h"
#include "../MessageSystem/observer.h"
#include "../MessageSystem/message.h"
#include "../FilePaths/filePath.h"


class ECSCoordinator : public GameSystems
{
public:
	ECSCoordinator() = default;
	~ECSCoordinator() = default;

	void initialise() override;
	void update() override;
	void cleanup() override;
	SystemType getSystem() override;

	//Entity Manager Functions
	Entity createEntity();
	/*Entity createEntity(const std::string& filename);*/
	void destroyEntity(Entity entity);
	
	//Component Manager Functions
	//Register the component
	template <typename T>
	void registerComponent();
	//Add components to entity
	template <typename T>
	void addComponent(Entity entity, T component);
	//Remove components from entity
	template <typename T>
	void removeComponent(Entity entity);
	//Get components from entity
	template <typename T>
	T& getComponent(Entity entity);
	//Get component type
	template <typename T>
	ComponentType getComponentType();

	//Helper Functions to ECSCoordinator
	//Clones the entity
	Entity cloneEntity(Entity entity);
	//Get number of live entities
	unsigned int getEntityNum();
	//Returns the first entity created
	Entity getFirstEntity();
	//Checks if entity has component
	template <typename T>
	bool hasComponent(Entity entity);
	//Helper function to get random value
	float getRandomVal(float min, float max);

	std::vector<Entity> getAllLiveEntities();
	std::string getEntityID(Entity entity);
	Entity getEntityFromID(std::string ID);
	void setEntityID(Entity entity, std::string ID);

	void LoadEntityFromJSON(ECSCoordinator& ecs, std::string const& filename);
	// save the entity's data to JSON file
	void SaveEntityToJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename);

	template <typename T>
	std::shared_ptr<T> getSpecificSystem();


	//System Manager Functions
	//Register the system
	template <typename T>
	std::shared_ptr<T> registerSystem();
	//Set the system signature
	template <typename T>
	void setSystemSignature(ComponentSig signature);

	ComponentSig getEntitySignature(Entity entity);

	void test5();
	void initialiseSystemsAndComponents();

private:
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<SystemManager> systemManager;

	Entity firstEntity;
};

//Register component by calling Component Manager Function
template <typename T>
void ECSCoordinator::registerComponent()
{
	componentManager->registerComponentHandler<T>();
}

//Add component to entity by calling Component Manager Function
//as well as updating the entity signature on the entity manager
template <typename T>
void ECSCoordinator::addComponent(Entity entity, T component)
{
	componentManager->addComponent<T>(entity, component);

	//update entity signature
	auto signature = entityManager->getSignature(entity);
	signature.set(componentManager->getComponentType<T>(), true);
	entityManager->setSignature(entity, signature);

	//update system signature
	systemManager->entitySigChange(entity, signature);
}


//Remove component from entity by calling Component Manager Function
//as well as updating the entity signature on the entity manager
template <typename T>
void ECSCoordinator::removeComponent(Entity entity)
{
	componentManager->removeComponent<T>(entity);

	//update entity signature
	auto signature = entityManager->getSignature(entity);
	signature.set(componentManager->getComponentType<T>(), false);
	entityManager->rmvSignature(entity, signature);

	//update system signature
	systemManager->entitySigChange(entity, signature);
}

//Get component from entity by calling Component Manager Function
template <typename T>
T& ECSCoordinator::getComponent(Entity entity)
{
	return componentManager->getComponent<T>(entity);
}

//Get component type by calling Component Manager Function
template <typename T>
ComponentType ECSCoordinator::getComponentType()
{
	return componentManager->getComponentType<T>();
}

//Register system by calling System Manager Function
template <typename T>
std::shared_ptr<T> ECSCoordinator::registerSystem()
{
	return systemManager->registerSystem<T>();
}

//Set system signature by calling System Manager Function
template <typename T>
void ECSCoordinator::setSystemSignature(ComponentSig signature)
{
	systemManager->setSystemSignature<T>(signature);
}

//Check if entity has component by calling Component Manager Function
template <typename T>
bool ECSCoordinator::hasComponent(Entity entity)
{
	return componentManager->hasComponent<T>(entity);
}

template <typename T>
std::shared_ptr<T> ECSCoordinator::getSpecificSystem()
{
	return systemManager->getSpecificSystem<T>();
}
