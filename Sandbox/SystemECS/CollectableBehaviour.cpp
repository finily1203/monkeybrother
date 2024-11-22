#include "CollectableBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

void CollectableBehaviour::update(Entity entity, float dt) {
	//get player entity based on ifPlayer component
	auto& playerEntity = entity;
	auto CollisionSystemRef = ecsCoordinator.getSpecificSystem<CollisionSystemECS>();
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
	auto& collectableTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
	myMath::Vector2D& playerPos = playerTransform.position;
	float radius = playerTransform.scale.GetX() * 0.5f;

	CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
	CollisionSystemECS::OBB platformOBB = collisionSystem.createOBBFromEntity(entity);

	myMath::Vector2D normal{};
	float penetration{};

	bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, platformOBB, normal, penetration);

	if (isColliding) {
		ecsCoordinator.destroyEntity(entity);
	}

}