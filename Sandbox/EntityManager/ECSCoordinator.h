#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "ECSDefinitions.h"


class ECSCoordinator
{
public:
	ECSCoordinator();

	//Entity Manager Functions
	Entity createEntity();
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

	//System Manager Functions
	template <typename T>
	std::shared_ptr<T> registerSystem();
	template <typename T>
	void setSystemSignature(ComponentSig signature);


private:
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<SystemManager> systemManager;
};