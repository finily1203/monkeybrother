/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   CollectableBehaviour.cpp
@brief:  This source file includes the implementation of the CollectableBehaviour
		 that logicSystemECS uses to handle the behaviour of the collectable entities.
		 Note that Collectables are affected by the Player entity

		 Joel Chu (c.weiyuan): defined the functions of CollectableBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#include "CollectableBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

void CollectableBehaviour::update(Entity entity) {
	//get player entity based on ifPlayer component
	
	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
	auto collisionSystem = PhysicsSystemRef->getCollisionSystem();


	for (auto& playerEntity : ecsCoordinator.getAllLiveEntities()) {
		if (ecsCoordinator.hasComponent<PlayerComponent>(playerEntity)) {

			//get the position of the player and the collectable
			auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
			myMath::Vector2D& playerPos = playerTransform.position;
			float radius = playerTransform.scale.GetX() * 0.5f;

			CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
			CollisionSystemECS::OBB collectOBB = collisionSystem.createOBBFromEntity(entity);

			myMath::Vector2D normal{};
			float penetration{};

			bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, collectOBB, normal, penetration);

			if (isColliding) {
				std::cout << "COLLIDED" << std::endl;
				//player grow in size and mass
				playerTransform.scale.SetX(playerTransform.scale.GetX() + 50.0f);
				playerTransform.scale.SetY(playerTransform.scale.GetY() + 50.0f);
				auto& playerPhysics = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity);
				playerPhysics.mass += 0.5f;

				GLFWFunctions::collectAudio = true;


				ecsCoordinator.destroyEntity(entity);
				GLFWFunctions::collectableCount--;
			}
		}
	}



}