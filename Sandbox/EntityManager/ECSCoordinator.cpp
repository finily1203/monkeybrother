#include "ECSCoordinator.h"

//extern ECSCoordinator* ecsCoordinator;

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

unsigned int ECSCoordinator::getEntityNum() {
	return entityManager->getLiveEntCount();
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

struct Position {
	myMath::Vector2D pos;

	void Serialize(Serializer::BaseSerializer& serializer, Serializer::SerializationMode mode)
	{
		float x, y;

		if (mode == Serializer::SerializationMode::READ)
		{
			ReadObjectStream(serializer, x, "Player.position.pos.x");
			ReadObjectStream(serializer, y, "Player.position.pos.y");

			pos.SetX(x);
			pos.SetY(y);
		}

		else if (mode == Serializer::SerializationMode::WRITE)
		{
			float x = pos.GetX();
			float y = pos.GetY();

			WriteObjectStream(serializer, x, "Player.position.pos.x");
			WriteObjectStream(serializer, y, "Player.position.pos.y");
		}
	}
};

struct Size {
	myMath::Vector2D scale;

	void Serialize(Serializer::BaseSerializer& serializer, Serializer::SerializationMode mode)
	{
		float x, y;

		if (mode == Serializer::SerializationMode::READ)
		{
			ReadObjectStream(serializer, x, "Player.size.scale.x");
			ReadObjectStream(serializer, y, "Player.size.scale.y");

			scale.SetX(x);
			scale.SetY(y);
		}

		else if (mode == Serializer::SerializationMode::WRITE)
		{
			float x = scale.GetX();
			float y = scale.GetY();

			WriteObjectStream(serializer, x, "Player.size.scale.x");
			WriteObjectStream(serializer, y, "Player.size.scale.y");
		}
	}
};

struct velocity {
	float speed;

	void Serialize(Serializer::BaseSerializer& serializer, Serializer::SerializationMode mode)
	{
		if (mode == Serializer::SerializationMode::READ)
		{
			ReadObjectStream(serializer, speed, "Player.velocity.speed");
		}

		else if (mode == Serializer::SerializationMode::WRITE)
		{
			WriteObjectStream(serializer, speed, "Player.velocity.speed");
		}
	}
};


class PlayerSystem : public System {
public:
	void initialise() override {
		std::cout << "PlayerSystem initialise" << std::endl;
	}

	void update(float dt) override {
		//std::cout << "PlayerSystem update" << std::endl;
	}

	void cleanup() override {
		std::cout << "PlayerSystem cleanup" << std::endl;
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

	Position position;
	serializer.ReadObject(position, "Player.position");
	ecs.addComponent(entity, position);

	Size size;
	serializer.ReadObject(size, "Player.size");
	ecs.addComponent(entity, size);

	velocity vel;
	serializer.ReadObject(vel, "Player.velocity");
	ecs.addComponent(entity, vel);
}

void ECSCoordinator::SaveEntityToJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename)
{
	Serializer::JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}

	// Get the current components of the entity
	if (ecs.entityManager->getSignature(entity).test(getComponentType<Position>()))
	{
		Position position = getComponent<Position>(entity);
		serializer.WriteObject(position, "Player.position");
	}

	if (ecs.entityManager->getSignature(entity).test(getComponentType<Size>()))
	{
		Size size = getComponent<Size>(entity);
		serializer.WriteObject(size, "Player.size");
	}

	if (ecs.entityManager->getSignature(entity).test(getComponentType<velocity>()))
	{
		velocity vel = getComponent<velocity>(entity);
		serializer.WriteObject(vel, "Player.velocity");
	}

	if (!serializer.Save(filename))
	{
		std::cout << "Error: could not save to file " << filename << std::endl;
	}
}

void ECSCoordinator::UpdateEntityPosition(Entity& entity, float x, float y)
{
	if (entityManager->getSignature(entity).test(getComponentType<Position>()))
	{
		Position& position = getComponent<Position>(entity);
		position.pos.SetX(x);
		position.pos.SetY(y);
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

	float x = 12.533f, y = -63.1567f;
	UpdateEntityPosition(loadedEntity, x, y);

	SaveEntityToJSON(*this, loadedEntity, "./Serialization/player.json");

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

Entity ECSCoordinator::cloneEntity(Entity entity)
{
	//Create new entity
	//Get all signatures of entity
	//check entity of each component and add into new entity
	//return new entity

	Entity newEntity = createEntity();

	auto signature = entityManager->getSignature(entity);

	for (auto i = 0u; i < MAX_COMPONENTS; i++)
	{
		if (signature[i])
		{
			if (i == getComponentType<Position>()) {
				componentManager->cloneComponent<Position>(entity, newEntity);
			}
			else if (i == getComponentType<Size>()) {
				componentManager->cloneComponent<Size>(entity, newEntity);
			}
			else if (i == getComponentType<velocity>()) {
				componentManager->cloneComponent<velocity>(entity, newEntity);
			}
		}

		return newEntity;
	}

	return newEntity;
}