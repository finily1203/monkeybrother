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
	auto& filterComponent = ecsCoordinator.getComponent<FilterComponent>(entity);

    for (auto& findPlayer : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<PlayerComponent>(findPlayer)) {
            playerEntity = findPlayer;
            break;
        }
    }

    auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
    myMath::Vector2D& playerPos = playerTransform.position;
    float radius = playerTransform.scale.GetX() * 0.5f;

    CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
    CollisionSystemECS::OBB filterOBB = collisionSystem.createOBBFromEntity(entity);

    myMath::Vector2D normal{};
    float penetration{};

    if (!isFilterUsed) {
        bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, filterOBB, normal, penetration);
        if (isColliding) {
            if (!filterComponent.isFilterClogged) {
                // Reduce size of mossball
                playerTransform.scale.SetX(playerTransform.scale.GetX() - 50.f);
                playerTransform.scale.SetY(playerTransform.scale.GetY() - 50.f);
                filterComponent.isFilterClogged = true;

                // Hide player temporarily
                auto& player = ecsCoordinator.getComponent<PlayerComponent>(playerEntity);
                player.isVisible = false;

                collisionTime = std::chrono::steady_clock::now();
            }

            if (filterComponent.isFilterClogged) {
                auto currentTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - collisionTime).count();

                if (duration >= 2) {
                    auto& player = ecsCoordinator.getComponent<PlayerComponent>(playerEntity);
                    player.isVisible = true;  // Make the player visible again

                    auto& filterPos = ecsCoordinator.getComponent<TransformComponent>(entity).position;
                    auto& filterScl = ecsCoordinator.getComponent<TransformComponent>(entity).scale;

                    // Manually normalize the collision normal
                    float magnitude = std::sqrt(normal.GetX() * normal.GetX() + normal.GetY() * normal.GetY());
                    myMath::Vector2D ejectDirection = (magnitude != 0.0f) ?
                        myMath::Vector2D(normal.GetX() / magnitude, normal.GetY() / magnitude) :
                        myMath::Vector2D(1.0f, 1.0f);  // Default upward push if zero

                    float ejectForceMagnitude = 0.f;  // Stronger ejection force

                    // Apply impulse force only once
                    auto& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).forceManager;
                    forceManager.AddForce(playerEntity, ejectDirection * ejectForceMagnitude);
                    forceManager.ApplyForce(playerEntity, ejectDirection, ejectForceMagnitude);

                    // Move player further to avoid immediate re-collision
                    playerPos = filterPos + ejectDirection * 100.0f + myMath::Vector2D(50.0f, (filterScl.GetY() * 0.4f));  // Increased distance
                    isFilterUsed = true;
                }
                else {
                    // Keep player hidden near the filter during the 2-second delay
                    auto& filterPos = ecsCoordinator.getComponent<TransformComponent>(entity).position;
                    auto& filterScl = ecsCoordinator.getComponent<TransformComponent>(entity).scale;
                    //playerPos = filterPos + myMath::Vector2D(50.0f, 0.f);
                    playerPos = filterPos + myMath::Vector2D(50.0f, (filterScl.GetY() * 0.4f));
                    //playerPos = filterPos + myMath::Vector2D(filterScl.GetX(), (filterScl.GetY() * 0.9f));
                }
            }
        }
    }
}