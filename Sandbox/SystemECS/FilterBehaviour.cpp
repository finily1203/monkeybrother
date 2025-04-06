/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   FilterBehaviour.cpp
@brief:  This source file includes the implementation of the FilterBehaviour
         that logicSystemECS uses to handle the behaviour of the filter entity.
         For our game implementation, the filter is meant to be a one-time use
         only object and will have visual indicators informing players that the filter
         can or cannot be used. Filter will reduce of the size of the mossball

         Joel Chu (c.weiyuan): defined the functions of FilterBehaviour class
                               100%
*//*___________________________________________________________________________-*/

#include "FilterBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"
#include "GUIGameViewport.h"

//filter needs to reduce size of mossball by one
//after filter is used, it is clogged so will use a diff texture

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
        if (GLFWFunctions::sizeIndex <= 0) {
            return;
        }
        bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, filterOBB, normal, penetration);
        if (isColliding) {
            if (!filterComponent.isFilterClogged) {
                // Reduce size of mossball
                playerTransform.scale.SetX(playerTransform.scale.GetX() - 25.f);
                playerTransform.scale.SetY(playerTransform.scale.GetY() - 25.f);
                filterComponent.isFilterClogged = true;

                // Hide player temporarily
                auto& player = ecsCoordinator.getComponent<PlayerComponent>(playerEntity);
                player.isVisible = false;

                collisionTime = std::chrono::steady_clock::now();
            }

            if (filterComponent.isFilterClogged) {
                auto currentTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - collisionTime).count();

                std::cout << duration << std::endl;

                if (duration >= 2) {
                    auto& player = ecsCoordinator.getComponent<PlayerComponent>(playerEntity);
                    player.isVisible = true;  // Make the player visible again

                    auto& filterPos = ecsCoordinator.getComponent<TransformComponent>(entity).position;
                    //auto& filterScl = ecsCoordinator.getComponent<TransformComponent>(entity).scale;

                    // Manually normalize the collision normal
                    float magnitude = std::sqrt(normal.GetX() * normal.GetX() + normal.GetY() * normal.GetY());
                    myMath::Vector2D ejectDirection = (magnitude != 0.0f) ?
                        myMath::Vector2D(normal.GetX() / magnitude, normal.GetY() / magnitude) :
                        myMath::Vector2D(1.0f, 1.0f);  // Default upward push if zero

                    float ejectForceMagnitude = 1.f;  // Stronger ejection force

                    // Apply impulse force only once
                    auto& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).forceManager;
                    forceManager.AddForce(playerEntity, ejectDirection * ejectForceMagnitude);
                    forceManager.ApplyForce(playerEntity, ejectDirection, ejectForceMagnitude);

					auto& filterScl = ecsCoordinator.getComponent<TransformComponent>(entity).scale;

                    if (filterScl.GetX() < 0.f)
                        playerPos.SetX(filterPos.GetX() - ejectDirection.GetX() * 10.f - 50.f);
                    else
                        playerPos.SetX(filterPos.GetX() + ejectDirection.GetX() * 10.f + 50.f);
                    playerPos.SetY(filterPos.GetY() + ejectDirection.GetY() * 10.f);
                    isFilterUsed = true;
                    GLFWFunctions::filterClogged = true;
                    GLFWFunctions::filterExitAudio = true;
                    GLFWFunctions::sizeIndex--;
                    GLFWFunctions::sizeChange = true;
					createCloggedAnimation(entity);
					createFilterPushAnimation(entity);
                }
                else {
                    // Keep player hidden near the filter during the 2-second delay
                    auto& filterPos = ecsCoordinator.getComponent<TransformComponent>(entity).position;
                    auto& filterScl = ecsCoordinator.getComponent<TransformComponent>(entity).scale;
                    //playerPos = filterPos + myMath::Vector2D(50.0f, 0.f);
					if (filterScl.GetX() < 0.f)
						playerPos = filterPos + myMath::Vector2D(-50.0f, (filterScl.GetY() * 0.4f));
					else
                        playerPos = filterPos + myMath::Vector2D(50.0f, 0.f);
                    //playerPos = filterPos + myMath::Vector2D(filterScl.GetX(), (filterScl.GetY() * 0.9f));
                }
            }
        }
    }
}

void FilterBehaviour::createCloggedAnimation(Entity entity) {
    Entity newAnimationEntity = ecsCoordinator.createEntity();

    ecsCoordinator.setEntityID(newAnimationEntity, "cloggedAnimation");
    ecsCoordinator.setTextureID(newAnimationEntity, "VFX_Finalised_DefunctFilter.png");

	int layer = layerManager.getEntityLayer(entity);

    // Transform setup
    TransformComponent transform{};
    auto& entityTransform = ecsCoordinator.getComponent<TransformComponent>(entity);

    transform.position = entityTransform.position;
    transform.scale.SetX(entityTransform.scale.GetX());
    transform.scale.SetY(entityTransform.scale.GetY());
	transform.orientation.SetX(entityTransform.orientation.GetX());

    ecsCoordinator.addComponent(newAnimationEntity, transform);

    // Animation setup
    AnimationComponent animation{};
    animation.isAnimated = true;
    animation.totalFrames = 8.0f;
    animation.frameTime = 0.05f;
    animation.columns = 3.0f;
    animation.rows = 3.0f;

    ecsCoordinator.addComponent(newAnimationEntity, animation);

    // Add to default layer 0
    layerManager.addEntityToLayer(layer, newAnimationEntity);
}

void FilterBehaviour::createFilterPushAnimation(Entity entity)
{
    Entity newAnimationEntity = ecsCoordinator.createEntity();

    ecsCoordinator.setEntityID(newAnimationEntity, "filterPush");
    ecsCoordinator.setTextureID(newAnimationEntity, "filter-out.png");

    int layer = layerManager.getEntityLayer(entity);

    // Transform setup
    TransformComponent transform{};
    auto& entityTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
    if (GameViewWindow::getSceneNum() == 12) {
        transform.position.SetX(entityTransform.position.GetX() + (entityTransform.scale.GetX() * 0.6f));
        transform.position.SetY(entityTransform.position.GetY() - (entityTransform.scale.GetY() * 0.5f));
	}
    else
    {
        transform.position.SetX(entityTransform.position.GetX() + (entityTransform.scale.GetX() * 0.8f));
        transform.position.SetY(entityTransform.position.GetY() + (entityTransform.scale.GetY() * 0.3f));
    }
    transform.scale.SetX(100.f);
    transform.scale.SetY(100.f);
    if (GameViewWindow::getSceneNum() == 5)
        transform.orientation.SetX(entityTransform.orientation.GetX());
    else
        transform.orientation.SetX(entityTransform.orientation.GetX() + 180.f);

    ecsCoordinator.addComponent(newAnimationEntity, transform);

    // Animation setup
    AnimationComponent animation{};
    animation.isAnimated = true;
    animation.totalFrames = 8.0f;
    animation.frameTime = 0.15f;
    animation.columns = 8.0f;
    animation.rows = 1.0f;

    ecsCoordinator.addComponent(newAnimationEntity, animation);

    // Add to default layer 0
    layerManager.addEntityToLayer(layer, newAnimationEntity);
}