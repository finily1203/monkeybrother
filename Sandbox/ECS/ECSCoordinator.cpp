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

#include "GraphicSystemECS.h"
#include "PhyColliSystemECS.h"
#include "LogicSystemECS.h"
#include "FontSystemECS.h"
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
	//if (GLFWFunctions::goNextMode) {
	//	for (Entity entity : entityManager->getLiveEntities()) {
	//		destroyEntity(entity);
	//	}
	//	//std::cout << getEntityNum() << std::endl;
	//	if (GLFWFunctions::testMode == 0) {
	//		test5();
	//	}
	//	else if (GLFWFunctions::testMode == 1) {
	//		test4();
	//	}
	//	GLFWFunctions::goNextMode = false;
	//}
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


//class PlayerSystem : public System {
//public:
//	void initialise() override {
//		std::cout << "PlayerSystem initialise" << std::endl;
//	}
//
//	void update(float dt) override {
//		//std::cout << "PlayerSystem update" << std::endl;
//	}
//
//	void cleanup() override {
//		std::cout << "PlayerSystem cleanup" << std::endl;
//	}
//};

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

		if (entityData.contains("aabb"))
		{
			AABBComponent aabb{};
			serializer.ReadObject(aabb.left, entityId, "entities.aabb.left");
			serializer.ReadObject(aabb.right, entityId, "entities.aabb.right");
			serializer.ReadObject(aabb.top, entityId, "entities.aabb.top");
			serializer.ReadObject(aabb.bottom, entityId, "entities.aabb.bottom");

			ecs.addComponent(entityObj, aabb);
		}

		if (entityData.contains("closestPlatform"))
		{
			ClosestPlatform closestPlatform{};
			serializer.ReadObject(closestPlatform.isClosest, entityId, "entities.closestPlatform.isClosest");

			ecs.addComponent(entityObj, closestPlatform);
		}

		if (entityData.contains("movement"))
		{
			MovementComponent movement{};
			serializer.ReadObject(movement.speed, entityId, "entities.movement.speed");

			ecs.addComponent(entityObj, movement);
		}

		if (entityData.contains("animation"))
		{
			AnimationComponent animation{};
			serializer.ReadObject(animation.isAnimated, entityId, "entities.animation.isAnimated");

			ecs.addComponent(entityObj, animation);
		}

		if (entityData.contains("enemy"))
		{
			EnemyComponent enemy{};
			serializer.ReadObject(enemy.isEnemy, entityId, "entities.enemy.isEnemy");

			ecs.addComponent(entityObj, enemy);
		}

		if (entityData.contains("forces"))
		{
			PhysicsComponent forces{};

			myMath::Vector2D direction = forces.force.GetDirection(); 
			float magnitude = forces.force.GetMagnitude();

			serializer.ReadObject(forces.mass, entityId, "entities.forces.mass");
			serializer.ReadObject(forces.gravityScale, entityId, "entities.forces.gravityScale");
			serializer.ReadObject(forces.jump, entityId, "entities.forces.jump");
			serializer.ReadObject(forces.dampening, entityId, "entities.forces.dampening");
			serializer.ReadObject(forces.velocity, entityId, "entities.forces.velocity");
			serializer.ReadObject(forces.maxVelocity, entityId, "entities.forces.maxVelocity");
			serializer.ReadObject(forces.acceleration, entityId, "entities.forces.acceleration");
			serializer.ReadObject(direction, entityId, "entities.forces.force.direction");
			serializer.ReadObject(magnitude, entityId, "entities.forces.force.magnitude");
			serializer.ReadObject(forces.accumulatedForce, entityId, "entities.forces.accumulatedForce");
			serializer.ReadObject(forces.maxAccumulatedForce, entityId, "entities.forces.maxAccumulatedForce");
			serializer.ReadObject(forces.prevForce, entityId, "entities.forces.prevForce");
			serializer.ReadObject(forces.targetForce, entityId, "entities.forces.targetForce");

			forces.force.SetDirection(direction);
			forces.force.SetMagnitude(magnitude);

			ecs.addComponent(entityObj, forces);
		}

		if (entityData.contains("font"))
		{
			FontComponent font{};
			serializer.ReadObject(font.text, entityId, "entities.font.text.string");
			serializer.ReadObject(font.textScale, entityId, "entities.font.textScale.scale");
			serializer.ReadObject(font.color, entityId, "entities.font.color");
			serializer.ReadObject(font.fontId, entityId, "entities.font.fontId.fontName");
			serializer.ReadObject(font.textBoxWidth, entityId, "entities.font.text.BoxWidth");

			std::cout << font.text << std::endl;
			std::cout << font.fontId << std::endl;

			ecs.addComponent(entityObj, font);
		}

		// set the entityId for the current entity
		ecs.entityManager->setEntityId(entityObj, entityId);
	}
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

			if (ecs.entityManager->getSignature(entity).test(getComponentType<AABBComponent>()))
			{
				AABBComponent aabb = getComponent<AABBComponent>(entity);

				serializer.WriteObject(aabb.left, entityId, "entities.aabb.left");
				serializer.WriteObject(aabb.right, entityId, "entities.aabb.right");
				serializer.WriteObject(aabb.top, entityId, "entities.aabb.top");
				serializer.WriteObject(aabb.bottom, entityId, "entities.aabb.bottom");
			}

			if (ecs.entityManager->getSignature(entity).test(getComponentType<ClosestPlatform>()))
			{
				ClosestPlatform closestPlatform = getComponent<ClosestPlatform>(entity);

				serializer.WriteObject(closestPlatform.isClosest, entityId, "entities.closestPlatform.isClosest");
			}

			if (ecs.entityManager->getSignature(entity).test(getComponentType<MovementComponent>()))
			{
				MovementComponent movement = getComponent<MovementComponent>(entity);

				serializer.WriteObject(movement.speed, entityId, "entities.movement.speed");
			}

			if (ecs.entityManager->getSignature(entity).test(getComponentType<AnimationComponent>()))
			{
				AnimationComponent animation = getComponent<AnimationComponent>(entity);

				serializer.WriteObject(animation.isAnimated, entityId, "entities.animation.isAnimated");
			}

			if (ecs.entityManager->getSignature(entity).test(getComponentType<EnemyComponent>()))
			{
				EnemyComponent enemy = getComponent<EnemyComponent>(entity);

				serializer.WriteObject(enemy.isEnemy, entityId, "entities.enemy.isEnemy");
			}

			if (ecs.entityManager->getSignature(entity).test(getComponentType<FontComponent>()))
			{
				FontComponent fontComp = getComponent<FontComponent>(entity);
				std::cout << "ECSCoordinator.cpp - [R: " << fontComp.color.GetX() << " G: " << fontComp.color.GetY() << " B: " << fontComp.color.GetZ() << "]" << std::endl;

				serializer.WriteObject(fontComp.text, entityId, "entities.font.text.string");
				serializer.WriteObject(fontComp.textBoxWidth, entityId, "entities.font.text.BoxWidth");
				serializer.WriteObject(fontComp.textScale, entityId, "entities.font.textScale.scale");
				serializer.WriteObject(fontComp.color, entityId, "entities.font.color");
				serializer.WriteObject(fontComp.fontId, entityId, "entities.font.fontId.fontName");
			}
		}
	}

	// checks if the JSON object is able to be saved to the JSON file 
	if (!serializer.Save(filename))
	{
		std::cout << "Error: could not save to file " << filename << std::endl;
	}
}

//clones the entity
Entity ECSCoordinator::cloneEntity(Entity entity)
{
	Entity newEntity = createEntity();

	if (entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent transform = getComponent<TransformComponent>(entity);
		//transform.position += glm::vec2(getRandomVal(-200.f, 800.f), getRandomVal(-200.f, 300.f));
		transform.position += myMath::Vector2D(getRandomVal(-200.f, 800.f), getRandomVal(-200.f, 300.f));
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
	//std::cout << "Create Platforms" << std::endl;
	//Entity platform1 = createEntity();
	//addComponent(platform1, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(500.f, 50.0f), glm::vec2(0.0f, -150.f) });
	//GraphicsComponent gfxComp1{};
	//gfxComp1.glObject.init(glm::vec2(0.0f, 0.0f), glm::vec2(500.f, 50.f), glm::vec2(0.0f, -150.0f));
	//addComponent(platform1, gfxComp1);
	//addComponent(platform1, AABBComponent{ -250.f, 250.f,-125.f, -175.f });
	//addComponent(platform1, ClosestPlatform{ false });

	//Entity platform2 = createEntity();
	//addComponent(platform2, TransformComponent{ glm::vec2(0.f, 0.f), glm::vec2(500.f, 50.f), glm::vec2(400.f, 200.f) });
	//GraphicsComponent gfxComp2{};
	//gfxComp2.glObject.init(glm::vec2(0.0f, 0.0f), glm::vec2(500.f, 50.f), glm::vec2(400.f, 200.0f));
	//addComponent(platform2, gfxComp2);
	//addComponent(platform2, AABBComponent{ 150.0f, 650.f, 225.f, 175.f });
	//addComponent(platform2, ClosestPlatform{ false });

	//Entity platform3 = createEntity();
	//addComponent(platform3, TransformComponent{ glm::vec2(315.f, 0.f), glm::vec2(300.f, 50.f), glm::vec2(-500.f, -200.f) });
	//GraphicsComponent gfxComp3{};
	//gfxComp3.glObject.init(glm::vec2(315.0f, 0.0f), glm::vec2(300.f, 50.f), glm::vec2(-500.f, -200.0f));
	//addComponent(platform3, gfxComp3);
	////addComponent(platform3, AABBComponent{ -350.0f, -650.f, -175.f, -225.f });
	//glm::vec2 platformPos = gfxComp3.glObject.position;
	//glm::vec2 platformScl = gfxComp3.glObject.scaling;
	//addComponent(platform3, AABBComponent{ platformPos.x - platformScl.x / 2, platformPos.x + platformScl.x / 2,
	//									   platformPos.y + platformScl.y / 2, platformPos.y - platformScl.y / 2 });
	//addComponent(platform3, ClosestPlatform{ false });

	//std::cout << "Create Player" << std::endl;
	//Entity player = createEntity();
	//addComponent(player, TransformComponent{ glm::vec2(0.0f, 0.f), glm::vec2(100.f, 100.f), glm::vec2(0.0f, 300.0f) });
	//GraphicsComponent gfxComp4{};
	//gfxComp4.glObject.init(glm::vec2(0.0f, 0.0f), glm::vec2(100.f, 100.f), glm::vec2(0.0f, 300.0f));
	//addComponent(player, gfxComp4);
	//addComponent(player, AABBComponent{ 1.f, 1.f, 1.f, 1.f });
	//addComponent(player, MovementComponent{ .02f });
}

//Test 5 tests to merge test 3 and test 4 (Physics and rendering without use of GLObject)
void ECSCoordinator::test5() {
	//LoadEntityFromJSON(*this, FilePathManager::GetEntitiesJSONPath());
	LoadEntityFromJSON(*this, FilePathManager::GetEntitiesJSONPath());

	/*int saveNum = DebugSystem::GetSaveCount();
	if (saveNum == 1)
	{
		LoadEntityFromJSON(*this, FilePathManager::GetEntitiesJSONPath());
	}
	 
	else
	{
		saveNum -= 1;
		LoadEntityFromJSON(*this, FilePathManager::GetSaveJSONPath(saveNum));
	}*/

	//std::string entityId = "player";
	//Entity entity = entityManager->getEntityById(entityId);

	//if (entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	//{
	//	TransformComponent& transform = getComponent<TransformComponent>(entity);
	//	// assign the new data of the transform component to the entity's transform component
	//	transform.position.SetX(120.f);
	//	transform.scale.SetY(45.f);
	//}

	//if (entityManager->getSignature(entity).test(getComponentType<MovementComponent>()))
	//{
	//	MovementComponent& movement = getComponent<MovementComponent>(entity);

	//	movement.speed = 0.1f;
	//}

	//SaveEntityToJSON(*this, entity, FilePathManager::GetEntitiesJSONPath());


	//create text entity
	//std::cout << "Create Text Entity 1" << std::endl;
	//Entity textEntity = createEntity();
	//addComponent(textEntity, TransformComponent{ myMath::Vector2D(0.0f, 0.f), myMath::Vector2D(0.f, 0.f), myMath::Vector2D(0.0f, 0.0f) });

	//FontComponent fontComp{};
	//fontComp.fontId = "Antonio";
	//fontComp.text = "Hello World!";
	//fontComp.textScale = 1.0f;
	//fontComp.color = MyMath::Vector3D(0.0f, 0.0f, 255.0f);

	////float screenWidth = 1600.0f;
	////float screenHeight = 900.0f;

	////float textScale = 1.0f;
	////glm::vec2 textSize = glm::vec2(200.0f * textScale, 50.0f * textScale);
	////fontComp.position = glm::vec2((screenWidth - textSize.x) / 2, (screenHeight - textSize.y) / 2 + 200); // Centered
	////fontComp.scale = textScale;
	////fontComp.color = glm::vec3(0.0f, 0.0f, 255.0f);

	//addComponent(textEntity, fontComp);

	//// Create another text entity
	//std::cout << "Create Text Entity 2" << std::endl;
	//Entity textEntity2 = createEntity();
	//addComponent(textEntity2, TransformComponent{ myMath::Vector2D(0.0f, 0.f), myMath::Vector2D(0.f, 0.f), myMath::Vector2D(0.0f, 0.0f) });

	//FontComponent fontComp2{};
	//fontComp2.fontPath = "Graphics/Assets/SS Journey.ttf";
	//fontComp2.text = "Hello this is another text with SS Journey.ttf. This is to check that text wrapping functionality works";

	//float textScale2 = 1.0f;
	//glm::vec2 textSize2 = glm::vec2(200.0f * textScale2, 50.0f * textScale2);

	//
	//fontComp2.position = glm::vec2((screenWidth - textSize.x) / 2, (screenHeight - textSize.y) / 2 - textSize2.y + 160); 
	//fontComp2.scale = textScale2;
	//fontComp2.color = glm::vec3(0.0f, 0.0f, 255.0f);

	//addComponent(textEntity2, fontComp2);

}


//Initialises all required components and systems for the ECS system
void ECSCoordinator::initialiseSystemsAndComponents() {
	std::cout << "Register Everything" << std::endl;
	//registerComponent<GraphicsComponent>();
	registerComponent<TransformComponent>();
	registerComponent<AABBComponent>();
	registerComponent<MovementComponent>();
	registerComponent<ClosestPlatform>();
	registerComponent<AnimationComponent>();
	registerComponent<EnemyComponent>();
	registerComponent<PhysicsComponent>();
	registerComponent<FontComponent>();

	//LOGIC MUST COME FIRST BEFORE PHYSICS FOLLOWED BY RENDERING

	auto logicSystem = std::make_shared<LogicSystemECS>();
	registerSystem<LogicSystemECS>();
	{
		ComponentSig logicSystemSig;
		logicSystemSig.set(getComponentType<TransformComponent>(), true);
	}

	logicSystem->initialise();

	auto physicsSystem = std::make_shared<PhysicsSystemECS>();
	registerSystem<PhysicsSystemECS>();
	{
		ComponentSig physicsSystemSig;
		physicsSystemSig.set(getComponentType<TransformComponent>(), true);
		physicsSystemSig.set(getComponentType<AABBComponent>(), true);
		physicsSystemSig.set(getComponentType<MovementComponent>(), true);
		physicsSystemSig.set(getComponentType<ClosestPlatform>(), true);
		physicsSystemSig.set(getComponentType<PhysicsComponent>(), true);
	}

	physicsSystem->initialise();

	auto graphicSystem = std::make_shared<GraphicSystemECS>();
	registerSystem<GraphicSystemECS>();
	{
		ComponentSig graphicSystemSig;
		graphicSystemSig.set(getComponentType<TransformComponent>(), true);
		graphicSystemSig.set(getComponentType<AnimationComponent>(), true);
		graphicSystemSig.set(getComponentType<EnemyComponent>(), false);
	}


	graphicSystem->initialise();

	auto fontSystemECS = std::make_shared<FontSystemECS>();

	registerSystem<FontSystemECS>();
	{
		ComponentSig fontSystemSig;
		fontSystemSig.set(getComponentType<TransformComponent>(), true);
		fontSystemSig.set(getComponentType<FontComponent>(), true);  
	}

	fontSystemECS->initialise();

	test5();
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
	
	//Iterate through the entities to find Object1 and Object2
	//for (auto entity : entityManager->getLiveEntities()) {
	//	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

	//	float left = transform.position.GetX() - transform.scale.GetX() / 2;
	//	float right = transform.position.GetX() + transform.scale.GetX() / 2;
	//	float top = transform.position.GetY() + transform.scale.GetY() / 2;
	//	float bottom = transform.position.GetY() - transform.scale.GetY() / 2;

	//	addComponent(entity, AABBComponent{left, right, top, bottom});
	//	if (entityManager->getEntityId(entity) == "Player") {
	//		addComponent(entity, MovementComponent{.1f});
	//	}
	//}
}

std::vector<Entity> ECSCoordinator::getAllLiveEntities() {
	return entityManager->getLiveEntities();
}

std::string ECSCoordinator::getEntityID(Entity entity) {
	return entityManager->getEntityId(entity);
}

Entity ECSCoordinator::getEntityFromID(std::string ID) {
	return entityManager->getEntityById(ID);
}

void ECSCoordinator::setEntityID(Entity entity, std::string ID) {
	entityManager->setEntityId(entity, ID);
}

ComponentSig ECSCoordinator::getEntitySignature(Entity entity) {
	return entityManager->getSignature(entity);
}
