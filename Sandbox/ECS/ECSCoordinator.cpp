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
						    UpdateEntityData functions that load data from JSON file to
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
#include "GraphicsSystem.h"
#include "AnimationComponent.h"

#include "GraphicSystemECS.h"
#include "PhyColliSystemECS.h"
#include "GraphicsSystem.h"
#include <Windows.h>

#include "GlobalCoordinator.h"
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
			test4();
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

SystemType ECSCoordinator::getSystem() {
	return SystemType::ECSType;
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
	//Entity entity = createEntity();
	LoadEntityFromJSON(*this, FilePathManager::GetEntitiesJSONPath());
	
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

	//Entity entity2 = createEntity();
	//LoadEntityFromJSON(*this, entity2, GetEnemyJSONPath());
}

struct Position {
	myMath::Vector2D pos;

	// serializing function for the first test on the ECS component so this function handles
	// both reading and writing of the position component
	void Serialize(BaseSerializer& serializer, SerializationMode mode)
	{
		float x, y;

		if (mode == SerializationMode::READ)
		{
			ReadObjectStream(serializer, x, "Player.position.pos.x");
			ReadObjectStream(serializer, y, "Player.position.pos.y");

			pos.SetX(x);
			pos.SetY(y);
		}

		else if (mode == SerializationMode::WRITE)
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
	void Serialize(BaseSerializer& serializer, SerializationMode mode)
	{
		float x, y;

		if (mode == SerializationMode::READ)
		{
			ReadObjectStream(serializer, x, "Player.size.scale.x");
			ReadObjectStream(serializer, y, "Player.size.scale.y");

			scale.SetX(x);
			scale.SetY(y);
		}

		else if (mode == SerializationMode::WRITE)
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
	void Serialize(BaseSerializer& serializer, SerializationMode mode)
	{
		if (mode == SerializationMode::READ)
		{
			ReadObjectStream(serializer, speed, "Player.velocity.speed");
		}

		else if (mode == SerializationMode::WRITE)
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
void ECSCoordinator::LoadEntityFromJSON(ECSCoordinator& ecs, std::string const& filename)
{
	JSONSerializer serializer;

	// checks if JSON file could be opened
	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}
	
	// return the JSON object from the file
	nlohmann::json jsonObj = serializer.GetJSONObject();

	// loop through the entities array in the JSON object and
	// retrieve each data
	for (const auto& entityData : jsonObj["entities"])
	{
		// create entities based on the number of entities in
		// entities array of the JSON object
		Entity entityObj = createEntity();
		TransformComponent transform{};

		// getting the entity Id of the current entity
		std::string entityId = entityData["id"].get<std::string>();

		// read all of the data from the JSON object and assign the data
		// to the current entity
		serializer.ReadObject(transform.position, entityId, "entities.transform.position");
		serializer.ReadObject(transform.scale, entityId, "entities.transform.scale");
		serializer.ReadObject(transform.orientation, entityId, "entities.transform.orientation");
		serializer.ReadObject(transform.mdl_xform, entityId, "entities.transform.localTransform");
		serializer.ReadObject(transform.mdl_to_ndc_xform, entityId, "entities.transform.projectionMatrix");

		// add the component with all of the data populated from
		// the JSON object
		ecs.addComponent(entityObj, transform);

		// set the entityId for the current entity
		ecs.entityManager->setEntityId(entityObj, entityId);
	}

	//ecs.addComponent(entity, MovementComponent{ 10.f });
}

// this function will save the entity's data to the JSON file
void ECSCoordinator::SaveEntityToJSON(ECSCoordinator& ecs, Entity& entity, std::string const& filename)
{
	JSONSerializer serializer;

	// checks if JSON file could be opened
	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}

	// returns the JSON object from the file
	nlohmann::json jsonObj = serializer.GetJSONObject();

	// loop through every entities in the array of the JSON object
	for (const auto& entityData : jsonObj["entities"])
	{
		// getting the id of the data in the entities array of
		// the JSON object
		std::string entityId = entityData["id"].get<std::string>();

		// this if statement is to change the particular entity data based on
		// the entityId. This will not change all data but just one
		if (ecs.entityManager->getEntityById(entityId) == entity)
		{
			// ensuring that entity has TransformComponent
			if (ecs.entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
			{
				TransformComponent transform = getComponent<TransformComponent>(entity);

				// change the data in the JSON object based on the entity's
				// data
				serializer.WriteObject(transform.position, entityId, "entities.transform.position");
				serializer.WriteObject(transform.scale, entityId, "entities.transform.scale");
				serializer.WriteObject(transform.orientation, entityId, "entities.transform.orientation");
				serializer.WriteObject(transform.mdl_xform, entityId, "entities.transform.localTransform");
				serializer.WriteObject(transform.mdl_to_ndc_xform, entityId, "entities.transform.projectionMatrix");
			}
		}
	}

	// checks if the JSON object is able to be saved to the JSON file
	if (!serializer.Save(filename))
	{
		std::cout << "Error: could not save to file " << filename << std::endl;
	}
}

// this function handles the updating of the entity's data
void ECSCoordinator::UpdateEntityData(Entity& entity)
{
	if (entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent& transform = getComponent<TransformComponent>(entity);
		// assign the new data of the transform component to the entity's transform component
		transform.position.x = 50.f;
		transform.scale.y = 20.f;
	}
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

	//if (entityManager->getSignature(entity).test(getComponentType<GraphicsComponent>()))
	//{
	//	GraphicsComponent graphics = getComponent<GraphicsComponent>(entity);
	//	graphics.glObject.position += glm::vec2(getRandomVal(-200.f, 800.f), getRandomVal(-200.f, 300.f));
	//	addComponent(newEntity, graphics);
	//}

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
	registerComponent<AnimationComponent>();

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
		graphicSystemSig.set(getComponentType<AnimationComponent>(), true);
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

//test new graphics System function
void ECSCoordinator::test4() {
	//Entity entObjGraphics = createEntity();
	////addComponent(entObjGraphics, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(50.f, 50.0f), glm::vec2(0.0f, -150.f), glm::mat3x3(1.0f), glm::mat3x3(1.0f) });
	//
	//LoadEntityFromJSON(*this, entObjGraphics, GetPlayerJSONPath());

	//Entity entObjGraphics2 = createEntity();
	//addComponent(entObjGraphics2, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(200.f, 200.0f), glm::vec2(300.0f, 0.f), glm::mat3x3(1.0f), glm::mat3x3(1.0f) });

	//Entity entObjGraphics3 = createEntity();
	//addComponent(entObjGraphics3, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(200.f, 200.0f), glm::vec2(-300.0f, 0.f), glm::mat3x3(1.0f), glm::mat3x3(1.0f) });

	LoadEntityFromJSON(*this, FilePathManager::GetEntitiesJSONPath());

	Observable fallEventSource("FallEventSource");
	Observer fallObserver("FallObserver");

	fallObserver.AttachHandler(MessageId::FALL, [](BaseMessage* msg) {
		FallMessage* fallMsg = static_cast<FallMessage*>(msg);
		Entity fallingEntity = fallMsg->GetPlayer();
		std::cout << "Observer: Entity " << fallMsg->GetId() << " is falling" << std::endl;
	});

	fallEventSource.Register(MessageId::FALL, &fallObserver);

	std::string id = "Player";
	Entity entity = entityManager->getEntityById(id);

	FallMessage fallMsg(entity);
	fallEventSource.ProcessMessage(&fallMsg);

	// This is the code for testing saving the entity data to the
	// JSON file
	//if (hasComponent<TransformComponent>(entity))
	//{
	//	UpdateEntityData(entity);

	//	SaveEntityToJSON(*this, entity, FilePathManager::GetEntitiesJSONPath());
	//}
	
	// Iterate through the entities to find Object1 and Object2
	for (auto entity : entityManager->getLiveEntities()) {
		std::string entityId = this->entityManager->getEntityId(entity); // Assume you have a method to get the entity ID

		/*if (entityId == "Object1" || entityId == "Object2") {
			addComponent(entity, AnimationComponent{ true });
		}*/
		/*std::cout << "------------------------------" << entityManager->getEntityID(entity) << std::endl;*/
	}
	Entity platform1 = createEntity();
	addComponent(platform1, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(1000.0f, 50.0f), glm::vec2(0.0f, -150.f) });
	/*std::cout <<"------------------------------" << entityManager->getEntityID(platform1) << std::endl;*/
}