#include "ECSCoordinator.h"

ECSCoordinator::ECSCoordinator()
{
	entityManager = std::make_unique<EntityManager>();
	componentManager = std::make_unique<ComponentManager>();
	systemManager = std::make_unique<SystemManager>();
}

Entity ECSCoordinator::createEntity()
{
	return entityManager->createEntity();
}

void ECSCoordinator::destroyEntity(Entity entity)
{
	//remove entity from all systems
	entityManager->destroyEntity(entity);
	componentManager->entityRemoved(entity);
	systemManager->entityRemoved(entity);
}

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
	entityManager->setSignature(entity, signature);

	//update system signature
	systemManager->entitySigChange(entity, signature);
}

template <typename T>
T& ECSCoordinator::getComponent(Entity entity)
{
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