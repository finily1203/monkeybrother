#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "ECSDefinitions.h"
#include "GlfwFunctions.h"

#include <iostream>
#include <fstream>
#include <vector>
#include "vector2D.h"
#include "../Serialization/jsonSerialization.h"
#include "../Serialization/serialization.h"


class ECSCoordinator : public Systems
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

	void LoadEntityFromJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename);
	void SaveJSONFromEntity(ECSCoordinator& ecs, Entity& entity, std::string const& filename);

	//System Manager Functions
	template <typename T>
	std::shared_ptr<T> registerSystem();
	template <typename T>
	void setSystemSignature(ComponentSig signature);

	void test();

private:
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<SystemManager> systemManager;
};

void ECSCoordinator::initialise() {
	entityManager = std::make_unique<EntityManager>();
	componentManager = std::make_unique<ComponentManager>();
	systemManager = std::make_unique<SystemManager>();
}

void ECSCoordinator::update() {
	systemManager->update();
}

void ECSCoordinator::cleanup() {
	entityManager->cleanup();
	componentManager->cleanup();
	systemManager->cleanup();
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
	entityManager->rmvSignature(entity, signature);

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

struct Position {
	myMath::Vector2D pos;

	void Serialize(Serializer::BaseSerializer& serializer)
	{
		float x, y;

		ReadObjectStream(serializer, x, "Player.position.pos.x");
		ReadObjectStream(serializer, y, "Player.position.pos.y");

		pos.SetX(x);
		pos.SetY(y);
	}
};

struct Size {
	myMath::Vector2D scale;

	void Serialize(Serializer::BaseSerializer& serializer)
	{
		float x, y;

		ReadObjectStream(serializer, x, "Player.size.scale.x");
		ReadObjectStream(serializer, y, "Player.size.scale.y");

		scale.SetX(x);
		scale.SetY(y);
	}
};

struct velocity {
	float speed;

	void Serialize(Serializer::BaseSerializer& serializer)
	{
		ReadObjectStream(serializer, speed, "Player.velocity.speed");
	}
};

class PlayerSystem : public System {
public:
	void update(float dt) override {
		std::cout << "PlayerSystem update" << std::endl;
	}
};

void ECSCoordinator::LoadEntityFromJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename)
{
	Serializer::JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}

	for (const auto& entityKey : serializer.GetEntityKeys())
	{
		Position position;
		serializer.ReadObject(position, entityKey + ".position");
		ecs.addComponent(entity, position);

		Size size;
		serializer.ReadObject(size, entityKey + ".size");
		ecs.addComponent(entity, size);

		velocity vel;
		serializer.ReadObject(vel, entityKey + ".velocity");
		ecs.addComponent(entity, vel);
	}
}

void ECSCoordinator::test() {
	std::cout << "testing ECS" << std::endl;
	//create player entity
	Entity player = createEntity();
	std::cout << "Number of available entities " << entityManager->getAvailableEntCount() << std::endl;
	std::cout << "Number of live entities " << entityManager->getLiveEntCount() << std::endl;

	//register components
	std::cout << "REGISTER TEST" << std::endl;
	registerComponent<Position>();
	registerComponent<Size>();
	registerComponent<velocity>();

	auto playerSystem = registerSystem<PlayerSystem>();

	addComponent(player, Position{ myMath::Vector2D(0, 0) });
	addComponent(player, Size{ myMath::Vector2D(1, 1) });
	addComponent(player, velocity{ 5 });
	std::cout << std::endl;

	//set system signature
	std::cout << "SET SYSTEM SIG TEST" << std::endl;
	ComponentSig playerSig;
	std::cout << "Player signature: " << playerSig << std::endl;
	playerSig.set(getComponentType<Position>(), true);
	playerSig.set(getComponentType<Size>(), true);
	playerSig.set(getComponentType<velocity>(), true);
	setSystemSignature<PlayerSystem>(playerSig);
	std::cout << std::endl;


	playerSystem->update(GLFWFunctions::delta_time);

	//get component
	std::cout << "RETRIEVE COMPONENT TEST" << std::endl;
	playerSig = entityManager->getSignature(player);
	std::cout << "Player signature: " << playerSig << std::endl;

	// Check if specific components are present
	bool hasPosition = playerSig.test(getComponentType<Position>());
	bool hasSize = playerSig.test(getComponentType<Size>());
	bool hasVelocity = playerSig.test(getComponentType<velocity>());

	// Retrieve components if they exist
	if (hasPosition) {
		Position playerPosition = getComponent<Position>(player);
		std::cout << "Player position: " << playerPosition.pos.GetX() << ", " << playerPosition.pos.GetY() << std::endl;
	}

	if (hasSize) {
		Size playerSize = getComponent<Size>(player);
		std::cout << "Player size: " << playerSize.scale.GetX() << ", " << playerSize.scale.GetY() << std::endl;
	}

	if (hasVelocity) {
		velocity playerVelocity = getComponent<velocity>(player);
		std::cout << "Player velocity: " << playerVelocity.speed << std::endl;
	}
	std::cout << std::endl;

	//remove component
	std::cout << "REMOVE COMPONENT TEST" << std::endl;
	removeComponent<Position>(player);
	removeComponent<Size>(player);
	removeComponent<velocity>(player);
	playerSig = entityManager->getSignature(player);
	std::cout << "Player signature: " << playerSig << std::endl << std::endl;

	//remove entity
	std::cout << "REMOVE ENTITY TEST" << std::endl;
	destroyEntity(player);
	std::cout << "Number of available entities " << entityManager->getAvailableEntCount() << std::endl;
	std::cout << "Number of live entities " << entityManager->getLiveEntCount() << std::endl;

	std::cout << std::endl;

	Entity loadedEntity = createEntity();
	LoadEntityFromJSON(*this, loadedEntity, "./Serialization/player.json");
	std::cout << "Number of available entities " << entityManager->getAvailableEntCount() << std::endl;
	std::cout << "Number of live entities " << entityManager->getLiveEntCount() << std::endl;

	playerSig.set(getComponentType<Position>(), true);
	playerSig.set(getComponentType<Size>(), true);
	playerSig.set(getComponentType<velocity>(), true);
	setSystemSignature<PlayerSystem>(playerSig);
	std::cout << std::endl;

	playerSystem->update(GLFWFunctions::delta_time);

	std::cout << "RETRIEVE COMPONENT TEST" << std::endl;
	playerSig = entityManager->getSignature(loadedEntity);
	std::cout << "Player signature: " << playerSig << std::endl;

	if (entityManager->getSignature(loadedEntity).test(getComponentType<Position>()))
	{
		Position loadedPos = getComponent<Position>(loadedEntity);
		std::cout << "Loaded Position from JSON: " << loadedPos.pos.GetX() << ", " << loadedPos.pos.GetY() << std::endl;
	}

	if (entityManager->getSignature(loadedEntity).test(getComponentType<Size>()))
	{
		Size loadedSize = getComponent<Size>(loadedEntity);
		std::cout << "Loaded Size from JSON: " << loadedSize.scale.GetX() << ", " << loadedSize.scale.GetY() << std::endl;
	}

	if (entityManager->getSignature(loadedEntity).test(getComponentType<velocity>()))
	{
		velocity loadedVel = getComponent<velocity>(loadedEntity);
		std::cout << "Loaded Velocity from JSON: " << loadedVel.speed << std::endl;
	}

	std::cout << std::endl;


	// remove component
	std::cout << "REMOVE COMPONENT TEST" << std::endl;
	removeComponent<Position>(loadedEntity);
	removeComponent<Size>(loadedEntity);
	removeComponent<velocity>(loadedEntity);
	playerSig = entityManager->getSignature(loadedEntity);
	std::cout << "Player signature: " << playerSig << std::endl << std::endl;

	// remove entity
	std::cout << "REMOVE ENTITY TEST" << std::endl;
	destroyEntity(loadedEntity);
	std::cout << "Number of available entities " << entityManager->getAvailableEntCount() << std::endl;
	std::cout << "Number of live entities " << entityManager->getLiveEntCount() << std::endl;
	std::cout << std::endl;
}