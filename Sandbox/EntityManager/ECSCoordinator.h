#pragma once
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


#include <iostream>
#include <fstream>
#include <vector>
#include "vector2D.h"
#include "matrix3x3.h"
#include "../Serialization/jsonSerialization.h"
#include "../Serialization/serialization.h"


class ECSCoordinator : public GameSystems
{
public:
	ECSCoordinator() = default;
	~ECSCoordinator() = default;

	void initialise() override;
	void update() override;
	void cleanup() override;

	//Entity Manager Functions
	Entity createEntity();
	/*Entity createEntity(const std::string& filename);*/
	void destroyEntity(Entity entity);
	
	//Component Manager Functions
	template <typename T>
	void registerComponent();
	template <typename T>
	void addComponent(Entity entity, T component);
	template <typename T>
	void removeComponent(Entity entity);
	template <typename T>
	T& getComponent(Entity entity);
	template <typename T>
	ComponentType getComponentType();

	//Clone Entity Function
	Entity cloneEntity(Entity entity);
	unsigned int getEntityNum();
	Entity getFirstEntity();

	void LoadEntityFromJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename);
	void SaveEntityToJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename);
	void UpdateEntity(Entity& entity, TransformComponent&, GraphicsComponent&);

	//System Manager Functions
	template <typename T>
	std::shared_ptr<T> registerSystem();
	template <typename T>
	void setSystemSignature(ComponentSig signature);

	void test();
	void test2();

private:
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<SystemManager> systemManager;

	Entity firstEntity;
};

template <typename T>
void ECSCoordinator::registerComponent()
{
	componentManager->registerComponentHandler<T>();
}

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

template <typename T>
T& ECSCoordinator::getComponent(Entity entity)
{
	//std::cout << "entity: " << entity << std::endl;
	//std::cout << "T: " << typeid(T).name() << std::endl;
	return componentManager->getComponent<T>(entity);
}

template <typename T>
ComponentType ECSCoordinator::getComponentType()
{
	return componentManager->getComponentType<T>();
}

template <typename T>
std::shared_ptr<T> ECSCoordinator::registerSystem()
{
	return systemManager->registerSystem<T>();
}

template <typename T>
void ECSCoordinator::setSystemSignature(ComponentSig signature)
{
	systemManager->setSystemSignature<T>(signature);
}

