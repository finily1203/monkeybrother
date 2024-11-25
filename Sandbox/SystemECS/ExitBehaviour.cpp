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

		if (isColliding) {
			std::cout << "Game Won" << std::endl;
		}
	}
}