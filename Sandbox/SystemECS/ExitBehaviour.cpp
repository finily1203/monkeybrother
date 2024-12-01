/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   ExitBehaviour.cpp
@brief:  This source file includes the implementation of the ExitBehaviour
		 that logicSystemECS uses to handle the behaviour of the exit entity.
		 Note that exit entity is affect by the collectable entities and the player
		 entity.

		 Joel Chu (c.weiyuan): defined the functions of ExitBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#include "ExitBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

void ExitBehaviour::update(Entity entity) {
	if (GLFWFunctions::collectableCount == 0) {
		auto playerEntity = ecsCoordinator.getEntityFromID("player");
		auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
		auto collisionSystem = PhysicsSystemRef->getCollisionSystem();


		for (auto& findPlayer : ecsCoordinator.getAllLiveEntities()) {
			if (ecsCoordinator.hasComponent<PlayerComponent>(findPlayer)) {
				playerEntity = findPlayer;
				break;
			}
		}

		//get the position of the player and the collectable
		auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
		myMath::Vector2D& playerPos = playerTransform.position;
		float radius = playerTransform.scale.GetX() * 0.5f;

		CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
		CollisionSystemECS::OBB exitOBB = collisionSystem.createOBBFromEntity(entity);

		myMath::Vector2D normal{};
		float penetration{};

		bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, exitOBB, normal, penetration);
		GLFWFunctions::exitCollision = isColliding;
		if (isColliding) {
			std::cout << "Game Won" << std::endl;
		}
	}
}