/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EffectPumpBehaviour.cpp
@brief:  This source file includes the implementation of the EffectPumpBehaviour
		 that logicSystemECS uses to handle the behaviour of the pump entities.
		 Pump entities will push the player entity and has a on and off duration

		 Joel Chu (c.weiyuan): defined the functions of EffectPumpBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#include "EffectPumpBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

	
void EffectPumpBehaviour::update(Entity entity) {
	timer += GLFWFunctions::delta_time;
	if (GLFWFunctions::isPumpOn && timer >= onDuration) {
		GLFWFunctions::isPumpOn = false;
		timer = 0.0f;
		std::cout << "Pump off" << std::endl;
	}
	else if (!GLFWFunctions::isPumpOn && timer >= offDuration) {
		GLFWFunctions::isPumpOn = true;
		timer = 0.0f;
		std::cout << "Pump on" << std::endl;
	}

	if (GLFWFunctions::isPumpOn) {
		auto playerEntity = ecsCoordinator.getEntityFromID("player");
		//auto bubblesEntity = ecsCoordinator.getEntityFromID("bubbles 3.png");
		auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
		auto collisionSystem = PhysicsSystemRef->getCollisionSystem();

		auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
		auto& bubblesTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
		myMath::Vector2D& playerPos = playerTransform.position;
		float radius = playerTransform.scale.GetX() * 0.5f;

		CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
		//CollisionSystemECS::OBB pumpOBB = collisionSystem.createOBBFromEntity(entity);
		CollisionSystemECS::OBB bubblesOBB = collisionSystem.createOBBFromEntity(entity);

		myMath::Vector2D normal{};
		float penetration{};
		bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, bubblesOBB, normal, penetration);

		if (isColliding) {
			myMath::Vector2D bubblesDirectionalVec = PhysicsSystemRef->directionalVector(bubblesTransform.orientation.GetX());
			auto& playerPhysics = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity);
			float pumpForce = ecsCoordinator.getComponent<PumpComponent>(entity).pumpForce;
			//float mag = playerPhysics.force.GetMagnitude() * pumpForce;
			ForceManager forceManager = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).forceManager;

			//forceManager.AddForce(playerEntity, myMath::Vector2D(-mag, -mag));
			forceManager.AddForce(playerEntity, bubblesDirectionalVec * pumpForce);

			std::cout << "Collision" << std::endl;
		}
	}
}