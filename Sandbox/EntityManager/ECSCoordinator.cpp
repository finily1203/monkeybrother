#include "ECSCoordinator.h"
#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "GraphicSystemECS.h"
#include "GraphicsSystem.h"
#include <Windows.h>


std::string ECSCoordinator::GetExecutablePath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	std::string fullPath(buffer);

	return fullPath.substr(0, fullPath.find_last_of("\\/"));
}

std::string ECSCoordinator::GetWindowConfigJSONPath()
{
	std::string execPath = GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\windowConfig.json";

	return jsonPath;
}

std::string ECSCoordinator::GetPlayerJSONPath()
{
	std::string execPath = GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\player.json";

	return jsonPath;
}

std::string ECSCoordinator::GetEnemyJSONPath()
{
	std::string execPath = GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\enemy.json";

	return jsonPath;
}

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

Entity ECSCoordinator::getFirstEntity() {
	return firstEntity;
}

Entity ECSCoordinator::createEntity()
{
	Entity newEntity = entityManager->createEntity();
	if(!firstEntity) {
		firstEntity = newEntity;	
	}
	return newEntity;
}

void ECSCoordinator::destroyEntity(Entity entity)
{
	//remove entity from all systems
	entityManager->destroyEntity(entity);
	componentManager->entityRemoved(entity);
	systemManager->entityRemoved(entity);
}

void ECSCoordinator::test2() {
	std::cout << "testing ECS with graphics side" << std::endl << std::endl;

	std::cout << "Registering component" << std::endl;
	registerComponent<TransformComponent>();
	registerComponent<GraphicsComponent>();

	std::cout << "Registering system and set Signature" << std::endl;
	auto graphicSystem = std::make_shared<GraphicSystemECS>();
	registerSystem<GraphicSystemECS>();
	{
		ComponentSig graphicSystemSig;
		graphicSystemSig.set(getComponentType<TransformComponent>(), true);
		graphicSystemSig.set(getComponentType<GraphicsComponent>(), true);
	}

	graphicSystem->initialise();

	std::cout << "Set entity" << std::endl;
	Entity entity = createEntity();
	LoadEntityFromJSON(*this, entity, GetPlayerJSONPath());
	
	// test codes for saving to JSON file
	// can be uncommented for the testing of saving to JSON file
	//TransformComponent transUpdate = getComponent<TransformComponent>(entity);
	//GraphicsComponent graphicsUpdate = getComponent<GraphicsComponent>(entity);
	//transUpdate.position = { 0.32f, -0.83f };
	//transUpdate.scale = { 0.5f, 0.5f };
	//transUpdate.orientation = { 0.0f, 0.0f };

	//graphicsUpdate.glObject.position = { -0.5f, -0.61f };
	//graphicsUpdate.glObject.scaling = { 0.15f, 0.3f };
	//graphicsUpdate.glObject.orientation = { 0.0f, 0.0f };

	//UpdateEntity(entity, transUpdate, graphicsUpdate);
	//SaveEntityToJSON(*this, entity, GetPlayerJSONPath());

	Entity entity2 = createEntity();
	LoadEntityFromJSON(*this, entity2, GetEnemyJSONPath());
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
	
	nlohmann::json jsonObj = serializer.GetJSONObject();
	std::string parentEntity = jsonObj.begin().key();

	TransformComponent transform{};
	serializer.ReadObject(transform.position, parentEntity + ".transform.position");
	serializer.ReadObject(transform.scale, parentEntity + ".transform.scale");
	serializer.ReadObject(transform.orientation, parentEntity + ".transform.orientation");
	ecs.addComponent(entity, transform);

	GraphicsComponent graphics{};
	serializer.ReadObject(graphics.glObject.position, parentEntity + ".graphics.position");
	serializer.ReadObject(graphics.glObject.scaling, parentEntity + ".graphics.scale");
	serializer.ReadObject(graphics.glObject.orientation, parentEntity + ".graphics.orientation");
	ecs.addComponent(entity, graphics);
}

void ECSCoordinator::SaveEntityToJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename)
{
	Serializer::JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();
	std::string parentEntity = jsonObj.begin().key();

	// Get the current components of the entity
	if (ecs.entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent transform = getComponent<TransformComponent>(entity);
		serializer.WriteObject(transform.position, parentEntity + ".transform.position");
		serializer.WriteObject(transform.scale, parentEntity + ".transform.scale");
		serializer.WriteObject(transform.orientation, parentEntity + ".transform.orientation");
	}

	if (ecs.entityManager->getSignature(entity).test(getComponentType<GraphicsComponent>()))
	{
		GraphicsComponent graphics = getComponent<GraphicsComponent>(entity);
		serializer.WriteObject(graphics.glObject.position, parentEntity + ".graphics.position");
		serializer.WriteObject(graphics.glObject.scaling, parentEntity + ".graphics.scale");
		serializer.WriteObject(graphics.glObject.orientation, parentEntity + ".graphics.orientation");
	}

	if (!serializer.Save(filename))
	{
		std::cout << "Error: could not save to file " << filename << std::endl;
	}
}

void ECSCoordinator::UpdateEntity(Entity& entity, TransformComponent& transUpdate, GraphicsComponent& graphicsUpdate)
{
	if (entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent& transform = getComponent<TransformComponent>(entity);
		transform.orientation = transUpdate.orientation;
		transform.position = transUpdate.position;
		transform.scale = transUpdate.scale;
	}

	if (entityManager->getSignature(entity).test(getComponentType<GraphicsComponent>()))
	{
		GraphicsComponent& graphics = getComponent<GraphicsComponent>(entity);
		graphics.glObject.position = graphicsUpdate.glObject.position;
		graphics.glObject.scaling = graphicsUpdate.glObject.scaling;
		graphics.glObject.orientation = graphicsUpdate.glObject.orientation;
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
	//UpdateEntityPosition(loadedEntity, x, y);

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
	Entity newEntity = createEntity();

	if(entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent transform = getComponent<TransformComponent>(entity);
		transform.position += glm::vec2(0.1f, 0.1f);
		addComponent(newEntity, transform);
	}

	if (entityManager->getSignature(entity).test(getComponentType<GraphicsComponent>()))
	{
		GraphicsComponent graphics = getComponent<GraphicsComponent>(entity);
		graphics.glObject.position += glm::vec2(0.1f, 0.1f);
		addComponent(newEntity, graphics);
	}

	return newEntity;
}