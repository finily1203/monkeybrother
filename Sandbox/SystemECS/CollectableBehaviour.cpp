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
    auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
    auto collisionSystem = PhysicsSystemRef->getCollisionSystem();

    // Check for collision with player
    for (auto& playerEntity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<PlayerComponent>(playerEntity)) {
            auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
            myMath::Vector2D& playerPos = playerTransform.position;
            float radius = playerTransform.scale.GetX() * 0.5f;

            CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
            CollisionSystemECS::OBB collectOBB = collisionSystem.createOBBFromEntity(entity);

            myMath::Vector2D normal{};
            float penetration{};

            bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, collectOBB, normal, penetration);

            if (isColliding) {
                playerTransform.scale.SetX(playerTransform.scale.GetX() + 25.0f);
                playerTransform.scale.SetY(playerTransform.scale.GetY() + 25.0f);
                auto& playerPhysics = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity);
                playerPhysics.mass += 0.5f;

                GLFWFunctions::collectAudio = true;

                createCollectAnimation(entity);

                ecsCoordinator.destroyEntity(entity);  // Destroy the collectable
                GLFWFunctions::collectableCount--;
            }
        }
    }
}

void CollectableBehaviour::createCollectAnimation(Entity entity) {
    Entity newAnimationEntity = ecsCoordinator.createEntity();

    ecsCoordinator.setEntityID(newAnimationEntity, "collectAnimation");
    ecsCoordinator.setTextureID(newAnimationEntity, "VFX_Finalised_CollectedMoss.png");

    // Transform setup
    TransformComponent transform{};
    auto& entityTransform = ecsCoordinator.getComponent<TransformComponent>(entity);

    transform.position = entityTransform.position;
    transform.scale.SetX(entityTransform.scale.GetX());
    transform.scale.SetY(entityTransform.scale.GetY());

    ecsCoordinator.addComponent(newAnimationEntity, transform);

    // Animation setup
    AnimationComponent animation{};
    animation.isAnimated = true;
    animation.totalFrames = 13.0f;
    animation.frameTime = 0.1f;
    animation.columns = 4.0f;
    animation.rows = 4.0f;

    ecsCoordinator.addComponent(newAnimationEntity, animation);

    // Add to default layer 0
    layerManager.addEntityToLayer(0, newAnimationEntity);
}
