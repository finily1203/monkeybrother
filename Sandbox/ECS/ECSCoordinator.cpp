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
#include "GUIGameViewport.h"

#include "PlayerBehaviour.h"
#include "EnemyBehaviour.h"
#include "CollectableBehaviour.h"
#include "EffectPumpBehaviour.h"
#include "ExitBehaviour.h"
#include "BehaviourComponent.h"
#include "BackgroundComponent.h"
#include "UIComponent.h"
#include "PlatformBehaviour.h"

#include <Windows.h>

#include "GlobalCoordinator.h"
#include <random>
#include <glm/glm.hpp>

//Initialise the ECS system and links the component manager, entity manager 
//and system manager
void ECSCoordinator::initialise() {
	entityManager = std::make_unique<EntityManager>();
	componentManager = std::make_unique<ComponentManager>();
	systemManager = std::make_unique<SystemManager>();
}

//Updates the ECS system
//based on the test modes it will render a different scene
void ECSCoordinator::update() {
	systemManager->update();

}

//Cleans up the ECS system by calling the cleanup function
//for the entity manager, component manager and system manager
void ECSCoordinator::cleanup() {
	if (systemManager) systemManager->cleanup();
	if (componentManager) componentManager->cleanup();
	if (entityManager) entityManager->cleanup();

	delete systemManager.release();
	delete componentManager.release();
	delete entityManager.release();
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
	/*if (!firstEntity) {
		firstEntity = newEntity;
	}*/
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

// this is the definition of the function that loads the data from JSON file to the entity
// open the JSON file and initialize the entity data based on the values read
void ECSCoordinator::LoadEntityFromJSON(ECSCoordinator& ecs, std::string const& filename)
{
	GLFWFunctions::collectableCount = 0;
	JSONSerializer serializer;

	// checks if JSON file could be opened
	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}
	
	// return the JSON object from the file
	nlohmann::json jsonObj = serializer.GetJSONObject();

	auto logicSystemRef = ecs.getSpecificSystem<LogicSystemECS>();

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
		std::string textureId = "none";
		if (entityData.contains("textureId")) {
			textureId = entityData["textureId"].get<std::string>();
		}
		//std::string textureId = entityData["textureId"].get<std::string>();

		// read all of the data from the JSON object and assign the data
		// to the current entity
		if (entityId != "placeholderentity") {
			serializer.ReadObject(transform.position, entityId, "entities.transform.position");
			serializer.ReadObject(transform.scale, entityId, "entities.transform.scale");
			serializer.ReadObject(transform.orientation, entityId, "entities.transform.orientation");
			serializer.ReadObject(transform.mdl_xform, entityId, "entities.transform.localTransform");
			serializer.ReadObject(transform.mdl_to_ndc_xform, entityId, "entities.transform.projectionMatrix");
		}

		// add the component with all of the data populated from
		// the JSON object
		ecs.addComponent(entityObj, transform);

		if (entityData.contains("background"))
		{
			BackgroundComponent background{};
			serializer.ReadObject(background.isBackground, entityId, "entities.background.isBackground");

			ecs.addComponent(entityObj, background);
		}

		if (entityData.contains("UI"))
		{
			UIComponent UI{};
			serializer.ReadObject(UI.isUI, entityId, "entities.UI.isUI");

			ecs.addComponent(entityObj, UI);
		}

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

		if (entityData.contains("player")) {
			PlayerComponent player{};
			serializer.ReadObject(player.isPlayer, entityId, "entities.player.isPlayer");

			ecs.addComponent(entityObj, player);

		}

		if (entityData.contains("enemy"))
		{
			EnemyComponent enemy{};
			serializer.ReadObject(enemy.isEnemy, entityId, "entities.enemy.isEnemy");

			ecs.addComponent(entityObj, enemy);

		}

		if (entityData.contains("collectable")) {

			CollectableComponent collectable{};
			serializer.ReadObject(collectable.isCollectable, entityId, "entities.collectable.isCollectable");

			ecs.addComponent(entityObj, collectable);


			GLFWFunctions::collectableCount++;
		}

		if (entityData.contains("pump")) {
			PumpComponent pump{};
			serializer.ReadObject(pump.isPump, entityId, "entities.pump.isPump");
			serializer.ReadObject(pump.pumpForce, entityId, "entities.pump.pumpForce");
			serializer.ReadObject(pump.isAnimate, entityId, "entities.pump.isAnimate");

			ecs.addComponent(entityObj, pump);

		}

		if (entityData.contains("exit")) {
			ExitComponent exit{};
			serializer.ReadObject(exit.isExit, entityId, "entities.exit.isExit");

			ecs.addComponent(entityObj, exit);

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

		if (entityData.contains("button"))
		{
			ButtonComponent button{};
			serializer.ReadObject(button.originalScale, entityId, "entities.transform.scale");
			serializer.ReadObject(button.hoveredScale, entityId, "entities.button.hoveredScale");
			serializer.ReadObject(button.isButton, entityId, "entities.button.isButton");

			ecs.addComponent(entityObj, button);
		}

		if (entityData.contains("behaviour")) {

			BehaviourComponent behaviour{};

			if (entityData["behaviour"].contains("none")) {
				serializer.ReadObject(behaviour.none, entityId, "entities.behaviour.none");
				logicSystemRef->unassignBehaviour(entityObj);
			}else
			if (entityData["behaviour"].contains("player")) {
				serializer.ReadObject(behaviour.player, entityId, "entities.behaviour.player");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<PlayerBehaviour>());
			}
			else
			if (entityData["behaviour"].contains("enemy")) {
				serializer.ReadObject(behaviour.enemy, entityId, "entities.behaviour.enemy");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<EnemyBehaviour>());
			}
			else
			if (entityData["behaviour"].contains("pump")) {
				serializer.ReadObject(behaviour.pump, entityId, "entities.behaviour.pump");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<EffectPumpBehaviour>());
			}
			else
			if (entityData["behaviour"].contains("exit")) {
				serializer.ReadObject(behaviour.exit, entityId, "entities.behaviour.exit");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<ExitBehaviour>());
			}
			else
			if (entityData["behaviour"].contains("collectable")) {
				serializer.ReadObject(behaviour.collectable, entityId, "entities.behaviour.collectable");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<CollectableBehaviour>());
			}
			else
			if (entityData["behaviour"].contains("button")) {
				serializer.ReadObject(behaviour.button, entityId, "entities.behaviour.button");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<MouseBehaviour>());
			}
			else
			if (entityData["behaviour"].contains("platform")) {
				serializer.ReadObject(behaviour.platform, entityId, "entities.behaviour.platform");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<PlatformBehaviour>());
			}

			ecs.addComponent(entityObj, behaviour);
		}

		// set the entityId for the current entity
		ecs.entityManager->setEntityId(entityObj, entityId);
		ecs.entityManager->setTextureId(entityObj, textureId);
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

			if (ecs.entityManager->getSignature(entity).test(getComponentType<PhysicsComponent>())) {
				PhysicsComponent physics = getComponent<PhysicsComponent>(entity);

				myMath::Vector2D direction = physics.force.GetDirection();
				float magnitude = physics.force.GetMagnitude();

				serializer.WriteObject(physics.mass, entityId, "entities.forces.mass");
				serializer.WriteObject(physics.gravityScale, entityId, "entities.forces.gravityScale");
				serializer.WriteObject(physics.jump, entityId, "entities.forces.jump");
				serializer.WriteObject(physics.dampening, entityId, "entities.forces.dampening");
				serializer.WriteObject(physics.velocity, entityId, "entities.forces.velocity");
				serializer.WriteObject(physics.maxVelocity, entityId, "entities.forces.maxVelocity");
				serializer.WriteObject(physics.acceleration, entityId, "entities.forces.acceleration");
				serializer.WriteObject(direction, entityId, "entities.forces.force.direction");
				serializer.WriteObject(magnitude, entityId, "entities.forces.force.magnitude");
				serializer.WriteObject(physics.accumulatedForce, entityId, "entities.forces.accumulatedForce");
				serializer.WriteObject(physics.maxAccumulatedForce, entityId, "entities.forces.maxAccumulatedForce");
				serializer.WriteObject(physics.prevForce, entityId, "entities.forces.prevForce");
				serializer.WriteObject(physics.targetForce, entityId, "entities.forces.targetForce");

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

				serializer.WriteObject(fontComp.text, entityId, "entities.font.text.string");
				serializer.WriteObject(fontComp.textBoxWidth, entityId, "entities.font.text.BoxWidth");
				serializer.WriteObject(fontComp.textScale, entityId, "entities.font.textScale.scale");
				serializer.WriteObject(fontComp.color, entityId, "entities.font.color");
				serializer.WriteObject(fontComp.fontId, entityId, "entities.font.fontId.fontName");
			}

			if (ecs.entityManager->getSignature(entity).test(getComponentType<ButtonComponent>()))
			{
				ButtonComponent button = getComponent<ButtonComponent>(entity);

				serializer.WriteObject(button.originalScale, entityId, "entities.transform.scale");
				serializer.WriteObject(button.hoveredScale, entityId, "entities.button.hoveredScale");
				serializer.WriteObject(button.isButton, entityId, "entities.button.isButton");
			}
		}
	}

	// checks if the JSON object is able to be saved to the JSON file 
	if (!serializer.Save(filename))
	{
		std::cout << "Error: could not save to file " << filename << std::endl;
	}
}

void ECSCoordinator::LoadMainMenuFromJSON(ECSCoordinator& ecs, std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		std::cout << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();

	auto logicSystemRef = ecs.getSpecificSystem<LogicSystemECS>();

	for (const auto& entityData : jsonObj["entities"])
	{
		Entity entityObj = createEntity();
		TransformComponent transform{};

		// getting the entity Id of the current entity
		std::string entityId = entityData["id"].get<std::string>();
		std::string textureId = entityData["textureId"].get<std::string>();

		// read all of the data from the JSON object and assign the data
		// to the current entity
		if (entityId != "placeholderentity") {
			serializer.ReadObject(transform.position, entityId, "entities.transform.position");
			serializer.ReadObject(transform.scale, entityId, "entities.transform.scale");
			serializer.ReadObject(transform.orientation, entityId, "entities.transform.orientation");
			serializer.ReadObject(transform.mdl_xform, entityId, "entities.transform.localTransform");
			serializer.ReadObject(transform.mdl_to_ndc_xform, entityId, "entities.transform.projectionMatrix");
		}

		// add the component with all of the data populated from
		// the JSON object
		ecs.addComponent(entityObj, transform);

		if (entityData.contains("background"))
		{
			BackgroundComponent background{};
			serializer.ReadObject(background.isBackground, entityId, "entities.background.isBackground");

			ecs.addComponent(entityObj, background);
		}

		if (entityData.contains("button"))
		{
			ButtonComponent button{};
			serializer.ReadObject(button.originalScale, entityId, "entities.transform.scale");
			serializer.ReadObject(button.hoveredScale, entityId, "entities.button.hoveredScale");
			serializer.ReadObject(button.isButton, entityId, "entities.button.isButton");

			ecs.addComponent(entityObj, button);
		}

		if (entityData.contains("behaviour"))
		{
			BehaviourComponent behaviour{};

			if (entityData["behaviour"].contains("none"))
			{
				serializer.ReadObject(behaviour.none, entityId, "entities.behaviour.none");
				logicSystemRef->unassignBehaviour(entityObj);
			}

			else if (entityData["behaviour"].contains("button"))
			{
				serializer.ReadObject(behaviour.button, entityId, "entities.behaviour.button");
				logicSystemRef->assignBehaviour(entityObj, std::make_shared<MouseBehaviour>());
			}

			ecs.addComponent(entityObj, behaviour);
		}

		ecs.entityManager->setEntityId(entityObj, entityId);
		ecs.entityManager->setTextureId(entityObj, textureId);
	}
	 
	GameViewWindow::setSceneNum(-1);
	GameViewWindow::SaveSceneToJSON(FilePathManager::GetSceneJSONPath());
}

//clones the entity
Entity ECSCoordinator::cloneEntity(Entity entity)
{
	Entity newEntity = createEntity();

	if (entityManager->getSignature(entity).test(getComponentType<TransformComponent>()))
	{
		TransformComponent transform = getComponent<TransformComponent>(entity);
		
		transform.position += myMath::Vector2D(getRandomVal(-200.f, 800.f), getRandomVal(-200.f, 300.f));
		addComponent(newEntity, transform);
	}

	return newEntity;
}

//Test 5 tests to merge test 3 and test 4 (Physics and rendering without use of GLObject)
void ECSCoordinator::test5() {
	//if (GameViewWindow::getSceneNum() != 0)
	//{
	//	int scene = GameViewWindow::getSceneNum();
	//	LoadEntityFromJSON(*this, FilePathManager::GetSaveJSONPath(scene));
	//}

	//else
	//{
	//	LoadEntityFromJSON(*this, FilePathManager::GetEntitiesJSONPath());
	//}

	LoadMainMenuFromJSON(*this, FilePathManager::GetMainMenuJSONPath());


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
	registerComponent<PlayerComponent>();
	registerComponent<ButtonComponent>();
	registerComponent<CollectableComponent>();
	registerComponent<PumpComponent>();
	registerComponent<ExitComponent>();
	registerComponent<BehaviourComponent>();
	registerComponent<BackgroundComponent>();
	registerComponent<UIComponent>();

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

std::vector<Entity> ECSCoordinator::getAllLiveEntities() {
	return entityManager->getLiveEntities();
}

std::string ECSCoordinator::getEntityID(Entity entity) {
	return entityManager->getEntityId(entity);
}

std::string ECSCoordinator::getTextureID(Entity entity) {
	return entityManager->getTextureId(entity);
}

Entity ECSCoordinator::getEntityFromID(std::string ID) {
	return entityManager->getEntityById(ID);
}

void ECSCoordinator::setEntityID(Entity entity, std::string ID) {
	entityManager->setEntityId(entity, ID);
}

void ECSCoordinator::setTextureID(Entity entity, std::string ID) {
	entityManager->setTextureId(entity, ID);
}

ComponentSig ECSCoordinator::getEntitySignature(Entity entity) {
	return entityManager->getSignature(entity);
}
