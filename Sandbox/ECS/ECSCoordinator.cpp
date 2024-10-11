/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan), Ian Loi (ian.loi)
@team:   MonkeHood
@course: CSD2401
@file:   ECSCoordinator.cpp
@brief:  This source file defines the functions found in the ECSCoordinator class.
		 Joel Chu (c.weiyuan): Defined most of the functions with regards to
							   the ECS system. This includes the creation,
							   updating and the destruction of objects.
							   70%
		 Ian Loi (ian.loi): Defined the LoadEntityFromJSON, SaveEntityToJSON and 
						    UpdateEntity functions that load data from JSON file to
							the entity, save data from entity to JSON file and update
							entity data respectively.
						    30%
*//*___________________________________________________________________________-*/

#include "ECSCoordinator.h"
#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "ClosestPlatform.h"
#include "GraphicSystemECS.h"
#include "PhyColliSystemECS.h"
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

#include <random>
#include <glm/glm.hpp>

//Initialise the ECS system and links the component manager, entity manager 
//and system manager
void ECSCoordinator::initialise() {
	entityManager = std::make_unique<EntityManager>();
	componentManager = std::make_unique<ComponentManager>();
	componentManager = std::make_unique<ComponentManager>();
	systemManager = std::make_unique<SystemManager>();
}

//Updates the ECS system
//based on the test modes it will render a different scene
void ECSCoordinator::update() {
	systemManager->update();
	

	if (GLFWFunctions::goNextMode) {
		for (Entity entity : entityManager->getLiveEntities()) {
			destroyEntity(entity);
		}
		if (GLFWFunctions::testMode == 0) {
			test3();
		}
		else if (GLFWFunctions::testMode == 1) {
			test2();
		}
		GLFWFunctions::goNextMode = false;
	}
}

//Cleans up the ECS system by calling the cleanup function
//for the entity manager, component manager and system manager
void ECSCoordinator::cleanup() {
	entityManager->cleanup();
	componentManager->cleanup();
	systemManager->cleanup();
}

//Returns the number of live entities
unsigned int ECSCoordinator::getEntityNum() {
	return entityManager->getLiveEntCount();
}

//Returns the first entity created
Entity ECSCoordinator::getFirstEntity() {
	return firstEntity;
}

//Creates entity by calling the entity manager function
//If there is no newEntity, the first entity created will be the newEntity
Entity ECSCoordinator::createEntity()
{
	Entity newEntity = entityManager->createEntity();
	if (!firstEntity) {
		firstEntity = newEntity;
	}
	return newEntity;
}

//Destroy the entity from all parts of the ECS system
void ECSCoordinator::destroyEntity(Entity entity)
{
	//remove entity from all systems
	entityManager->destroyEntity(entity);
	componentManager->entityRemoved(entity);
	systemManager->entityRemoved(entity);
}


//Used to test for serialization of entities;
void ECSCoordinator::test2() {
	std::cout << "testing ECS with graphics side" << std::endl << std::endl;

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

	// serializing function for the first test on the ECS component so this function handles
	// both reading and writing of the position component
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

	// serializing function for the first test on the ECS component so this function handles
	// both reading and writing of the size component
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

	// serializing function for the first test on the ECS component so this function handles
	// both reading and writing of the velocity component
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

// this is the definition of the function that loads the data from JSON file to the entity
// open the JSON file and initialize the entity data based on the values read
void ECSCoordinator::LoadEntityFromJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename)
{
	Serializer::JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}
	
	// return the JSON object from the file
	nlohmann::json jsonObj = serializer.GetJSONObject();
	// finding the parent entity key of the JSON object
	std::string parentEntity = jsonObj.begin().key();

	TransformComponent transform{};
	// read transform component from the JSON file
	// read the position first
	serializer.ReadObject(transform.position, parentEntity + ".transform.position");
	// read the scale next
	serializer.ReadObject(transform.scale, parentEntity + ".transform.scale");
	// read the orientation last
	serializer.ReadObject(transform.orientation, parentEntity + ".transform.orientation");
	ecs.addComponent(entity, transform);

	GraphicsComponent graphics{};
	// read graphics component from the JSON file
	// read the posution first
	serializer.ReadObject(graphics.glObject.position, parentEntity + ".graphics.position");
	// read the scale next
	serializer.ReadObject(graphics.glObject.scaling, parentEntity + ".graphics.scale");
	// read the orientation last
	serializer.ReadObject(graphics.glObject.orientation, parentEntity + ".graphics.orientation");
	ecs.addComponent(entity, graphics);

	//ecs.addComponent(entity, MovementComponent{ 10.f });
}

// this function will save the entity's data to the JSON file
void ECSCoordinator::SaveEntityToJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename)
{
	Serializer::JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}

	// returns the JSON object from the file
	nlohmann::json jsonObj = serializer.GetJSONObject();
	// finding the parent entity key of the JSON object
	std::string parentEntity = jsonObj.begin().key();

	// Get the current components of the entity
	if (ecs.entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent transform = getComponent<TransformComponent>(entity);
		// write the transform component to the JSON file
		serializer.WriteObject(transform.position, parentEntity + ".transform.position");
		serializer.WriteObject(transform.scale, parentEntity + ".transform.scale");
		serializer.WriteObject(transform.orientation, parentEntity + ".transform.orientation");
	}

	if (ecs.entityManager->getSignature(entity).test(getComponentType<GraphicsComponent>()))
	{
		// write the graphics component to the JSON file
		GraphicsComponent graphics = getComponent<GraphicsComponent>(entity);
		serializer.WriteObject(graphics.glObject.position, parentEntity + ".graphics.position");
		serializer.WriteObject(graphics.glObject.scaling, parentEntity + ".graphics.scale");
		serializer.WriteObject(graphics.glObject.orientation, parentEntity + ".graphics.orientation");
	}

	// saving to JSON file failed execute this block
	if (!serializer.Save(filename))
	{
		std::cout << "Error: could not save to file " << filename << std::endl;
	}
}

// this function handles the updating of the entity's data
void ECSCoordinator::UpdateEntity(Entity& entity, TransformComponent& transUpdate, GraphicsComponent& graphicsUpdate)
{
	if (entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent& transform = getComponent<TransformComponent>(entity);
		// assign the new data of the transform component to the entity's transform component
		transform.orientation = transUpdate.orientation;
		transform.position = transUpdate.position;
		transform.scale = transUpdate.scale;
	}

	if (entityManager->getSignature(entity).test(getComponentType<GraphicsComponent>()))
	{
		GraphicsComponent& graphics = getComponent<GraphicsComponent>(entity);
		// assign the new data of the graphics component to the entity's graphics component
		graphics.glObject.position = graphicsUpdate.glObject.position;
		graphics.glObject.scaling = graphicsUpdate.glObject.scaling;
		graphics.glObject.orientation = graphicsUpdate.glObject.orientation;
	}
}

//First ever test to test the ECS system
//Currently no longer in use as it is used for testing purposes
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

//clones the entity
Entity ECSCoordinator::cloneEntity(Entity entity)
{
	Entity newEntity = createEntity();

	if (entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent transform = getComponent<TransformComponent>(entity);
		transform.position += glm::vec2(getRandomVal(-200.f, 800.f), getRandomVal(-200.f, 300.f));
		addComponent(newEntity, transform);
	}

	if (entityManager->getSignature(entity).test(getComponentType<GraphicsComponent>()))
	{
		GraphicsComponent graphics = getComponent<GraphicsComponent>(entity);
		graphics.glObject.position += glm::vec2(getRandomVal(-200.f, 800.f), getRandomVal(-200.f, 300.f));
		addComponent(newEntity, graphics);
	}

	return newEntity;
}

//Test 3 tests for the creation of platforms and works with the physics and collision system
void ECSCoordinator::test3() {
	std::cout << "Create Platforms" << std::endl;
	Entity platform1 = createEntity();
	addComponent(platform1, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(500.f, 50.0f), glm::vec2(0.0f, -150.f) });
	GraphicsComponent gfxComp1{};
	gfxComp1.glObject.init(glm::vec2(0.0f, 0.0f), glm::vec2(500.f, 50.f), glm::vec2(0.0f, -150.0f));
	addComponent(platform1, gfxComp1);
	addComponent(platform1, AABBComponent{ -250.f, 250.f,-125.f, -175.f });
	addComponent(platform1, ClosestPlatform{ false });

	Entity platform2 = createEntity();
	addComponent(platform2, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(500.f, 50.f), glm::vec2(400.f, 200.f) });
	GraphicsComponent gfxComp2{};
	gfxComp2.glObject.init(glm::vec2(0.0f, 0.0f), glm::vec2(500.f, 50.f), glm::vec2(400.f, 200.0f));
	addComponent(platform2, gfxComp2);
	addComponent(platform2, AABBComponent{ 150.0f, 650.f, 225.f, 175.f });
	addComponent(platform2, ClosestPlatform{ false });

	Entity platform3 = createEntity();
	addComponent(platform3, TransformComponent{ glm::vec2(315.f, 0.f), glm::vec2(300.f, 50.f), glm::vec2(-500.f, -200.f) });
	GraphicsComponent gfxComp3{};
	gfxComp3.glObject.init(glm::vec2(315.0f, 0.0f), glm::vec2(300.f, 50.f), glm::vec2(-500.f, -200.0f));
	addComponent(platform3, gfxComp3);
	//addComponent(platform3, AABBComponent{ -350.0f, -650.f, -175.f, -225.f });
	glm::vec2 platformPos = gfxComp3.glObject.position;
	glm::vec2 platformScl = gfxComp3.glObject.scaling;
	addComponent(platform3, AABBComponent{ platformPos.x - platformScl.x / 2, platformPos.x + platformScl.x / 2,
										   platformPos.y + platformScl.y / 2, platformPos.y - platformScl.y / 2 });
	addComponent(platform3, ClosestPlatform{ false });

	std::cout << "Create Player" << std::endl;
	Entity player = createEntity();
	addComponent(player, TransformComponent{ glm::vec2(0.0f, 0.f), glm::vec2(100.f, 100.f), glm::vec2(0.0f, 300.0f) });
	GraphicsComponent gfxComp4{};
	gfxComp4.glObject.init(glm::vec2(0.0f, 0.0f), glm::vec2(100.f, 100.f), glm::vec2(0.0f, 300.0f));
	addComponent(player, gfxComp4);
	addComponent(player, AABBComponent{ 1.f, 1.f, 1.f, 1.f });
	addComponent(player, MovementComponent{ .02f });

}


//Initialises all required components and systems for the ECS system
void ECSCoordinator::initialiseSystemsAndComponents() {
	std::cout << "Register Everything" << std::endl;
	registerComponent<GraphicsComponent>();
	registerComponent<TransformComponent>();
	registerComponent<AABBComponent>();
	registerComponent<MovementComponent>();
	registerComponent<ClosestPlatform>();

	auto physicsSystem = std::make_shared<PhysicsSystemECS>();
	registerSystem<PhysicsSystemECS>();
	{
		ComponentSig physicsSystemSig;
		physicsSystemSig.set(getComponentType<TransformComponent>(), true);
		physicsSystemSig.set(getComponentType<AABBComponent>(), true);
		physicsSystemSig.set(getComponentType<MovementComponent>(), true);
		physicsSystemSig.set(getComponentType<ClosestPlatform>(), true);
		physicsSystemSig.set(getComponentType<GraphicsComponent>(), true);
	}

	physicsSystem->initialise();

	auto graphicSystem = std::make_shared<GraphicSystemECS>();
	registerSystem<GraphicSystemECS>();
	{
		ComponentSig graphicSystemSig;
		graphicSystemSig.set(getComponentType<TransformComponent>(), true);
		graphicSystemSig.set(getComponentType<GraphicsComponent>(), true);
	}


	graphicSystem->initialise();
}


//Helper function to get random value for the cloning object
float ECSCoordinator::getRandomVal(float min = -100.0f, float max = 100.0f) {
	std::random_device rd;  
	std::mt19937 gen(rd()); 
	std::uniform_real_distribution<float> dis(min, max); 
	return dis(gen); 
}