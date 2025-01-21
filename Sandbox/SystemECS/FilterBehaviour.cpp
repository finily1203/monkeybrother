#include "FilterBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

//filter needs to reduce size of mossball by one
//after filter is used, it is clogged so will use a diff texture


//steps to take
//get player entity position
//get filter entity position
//check if player entity collides with filter entity
//only if it collides, reduce size of mossball by one
//set isClogged to true

void FilterBehaviour::update(Entity entity) {
	auto playerEntity = ecsCoordinator.getEntityFromID("player");
	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
	auto collisionSystem = PhysicsSystemRef->getCollisionSystem();


	for (auto& findPlayer : ecsCoordinator.getAllLiveEntities()) {
		if (ecsCoordinator.hasComponent<PlayerComponent>(findPlayer)) {
			playerEntity = findPlayer;
			break;
		}
	}

	//get the position of the player and the filter
	auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
	myMath::Vector2D& playerPos = playerTransform.position;
	float radius = playerTransform.scale.GetX() * 0.5f;

	CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
	CollisionSystemECS::OBB filterOBB = collisionSystem.createOBBFromEntity(entity);

	myMath::Vector2D normal{};
	float penetration{};

	bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, filterOBB, normal, penetration);
	if (isColliding) {
		if (!isClogged) {
			//std::cout << "Filter collided" << std::endl;
			//reduce size of mossball by one
			auto& mossballTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
			mossballTransform.scale.SetX(mossballTransform.scale.GetX() - 50.f);
			mossballTransform.scale.SetY(mossballTransform.scale.GetY() - 50.f);
			isClogged = true;
		}
	}
}