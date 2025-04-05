/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   ExitBehaviour.cpp
@brief:  This source file implements the ExitBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the exit entity.

         Joel Chu (c.weiyuan): defined the ExitBehaviour class
                               100%
*//*___________________________________________________________________________-*/

#include "ExitBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"
#include "GUIGameViewport.h"
#include "NavigationArrow.h"

void ExitBehaviour::update(Entity entity) {
    static bool exitArrowCreated = false;
    static bool previousCollectableState = true;
    bool currentCollectableState = GLFWFunctions::collectableCount > 0;

    // Check if the collectable state has changed (all collectables are collected)
    if (previousCollectableState != currentCollectableState && currentCollectableState == false) {
        // All collectables were just collected, create or show the exit arrow
        if (!exitArrowCreated) {
            // Create navigation arrow for exit with special texture
            NavigationArrow::CreateNavigationArrow(entity);
            exitArrowCreated = true;

            // Find the arrow entity and change its texture to exit_arrow
            for (auto& arrowEntity : ecsCoordinator.getAllLiveEntities()) {
                if (ecsCoordinator.hasComponent<NavigationComponent>(arrowEntity) &&
                    ecsCoordinator.getEntityID(arrowEntity) == "nav_arrow") {
                    // Check if this is the arrow pointing to our exit
                    auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(arrowEntity);
                    if (navComp.isVisible) {
                        // Change the texture to exit_arrow instead of the default nav_arrow
                        ecsCoordinator.setTextureID(arrowEntity, "exit_arrow");
                        break;
                    }
                }
            }
        }
        else {
            // Show existing arrow (Navigation system will handle this automatically
            // when we call CreateNavigationArrow for an existing target)
            NavigationArrow::CreateNavigationArrow(entity);

            // Ensure the arrow is using the exit_arrow texture
            for (auto& arrowEntity : ecsCoordinator.getAllLiveEntities()) {
                if (ecsCoordinator.hasComponent<NavigationComponent>(arrowEntity) &&
                    ecsCoordinator.getEntityID(arrowEntity) == "nav_arrow") {
                    // Check if this is the arrow pointing to our exit
                    auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(arrowEntity);
                    if (navComp.isVisible) {
                        // Change the texture to exit_arrow instead of the default nav_arrow
                        ecsCoordinator.setTextureID(arrowEntity, "exit_arrow");
                        break;
                    }
                }
            }
        }
    }

    previousCollectableState = currentCollectableState;

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

        // Get the position of the player and the exit
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
            if (!GLFWFunctions::changeLevel) {
                GLFWFunctions::gamePaused = true;

                if (GLFWFunctions::levelCompletedMenuCount < 1)
                {
                    ecsCoordinator.LoadLevelCompletedMenuFromJSON(ecsCoordinator, FilePathManager::GetLevelCompletedMenuJSONPath());
                    GLFWFunctions::levelCompletedMenuCount++;
                }
                //int currScn = GameViewWindow::getSceneNum();
                //currScn++;
                //if (currScn > 2) currScn = -1;
                //GameViewWindow::setSceneNum(currScn);
                //GLFWFunctions::changeLevel = true;
                //GLFWFunctions::newSceneLoaded = true;

                //// Reset exit arrow state for next level
                //exitArrowCreated = false;
            }
        }

        if (GLFWFunctions::instantWin) {
			auto& entityTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
			playerTransform.position = entityTransform.position;
			GLFWFunctions::instantWin = false;
        }
    }
}