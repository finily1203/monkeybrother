/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   GraphicSystemECS.cpp
@brief:  This source file defines the functions in GraphicSystemECS class.
         This class is used to handle the communication between ECS and graphic
         system.
         Joel Chu (c.weiyuan): Defined the functions in GraphicSystemECS class.
                               Most of the content in this file is taken from
                               WindowSystem.cpp and modified to fit the scope
                               for creating object in ECS.
                               100%
*//*___________________________________________________________________________-*/
#include "GraphicSystemECS.h"

#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "AnimationComponent.h"
#include "EnemyComponent.h"
#include "PhysicsComponent.h"
#include "BehaviourComponent.h"
#include "BackgroundComponent.h"
#include "UIComponent.h"

#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include "Debug.h"
#include "GUIConsole.h"
#include "vector"



//std::unique_ptr<EntityManager> entityManager;
//Initialise currently does not do anything
void GraphicSystemECS::initialise() {

}

//Update function to update the graphics system
//uses functions from GraphicsSystem class to update, draw
//and render objects.
void GraphicSystemECS::update(float dt) {


    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        // Check if the entity has a transform component
        auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
        Console::GetLog() << "Entity: " << entity << " Position: " << transform.position.GetX() << ", " << transform.position.GetY() << std::endl;

        // Check if the entity has an animation component
        auto& animation = ecsCoordinator.getComponent<AnimationComponent>(entity);
        Console::GetLog() << "Entity: " << entity << " Animation: " << (animation.isAnimated ? "True" : "False") << std::endl;

        auto entitySig = ecsCoordinator.getEntitySignature(entity);

        bool isPlayer = ecsCoordinator.hasComponent<PlayerComponent>(entity);
        bool isEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);
        bool hasMovement = ecsCoordinator.hasComponent<PhysicsComponent>(entity);
        bool isBackground = ecsCoordinator.hasComponent<BackgroundComponent>(entity);
        bool isPlatform = ecsCoordinator.hasComponent<ClosestPlatform>(entity);
        bool isButton = ecsCoordinator.hasComponent<ButtonComponent>(entity);
		bool isCollectable = ecsCoordinator.hasComponent<CollectableComponent>(entity);
		bool isPump = ecsCoordinator.hasComponent<PumpComponent>(entity);
		bool isExit = ecsCoordinator.hasComponent<ExitComponent>(entity);
        bool isUI = ecsCoordinator.hasComponent<UIComponent>(entity);
        bool isAnimate = false;

        if (ecsCoordinator.hasComponent<PumpComponent>(entity)) {
            const auto& pumpComponent = ecsCoordinator.getComponent<PumpComponent>(entity);
            isAnimate = pumpComponent.isAnimate;
        }

        // Use hasMovement for the update parameter
        graphicsSystem.Update(dt / 10.0f, (isAnimate&& isPump) || (isPlayer && hasMovement) || (isEnemy && hasMovement)); // Use hasMovement instead of true
        myMath::Matrix3x3 identityMatrix = { 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f };
        transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, cameraSystem.getViewMatrix());

        // Compute view matrix
        if (GLFWFunctions::allow_camera_movement) { // Press F2 to allow camera movement
            cameraSystem.update();
        }
        else if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
            cameraSystem.lockToComponent(transform);
            cameraSystem.update();
        }

        // TODO:: Update AABB component inside game loop
        // Press F1 to draw out debug AABB
        if (GLFWFunctions::debug_flag && !ecsCoordinator.hasComponent<FontComponent>(entity) && !ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
            if (ecsCoordinator.getEntityID(entity) == "quitButton" || ecsCoordinator.getEntityID(entity) == "retryButton")
            {
                graphicsSystem.drawDebugOBB(ecsCoordinator.getComponent<TransformComponent>(entity), identityMatrix);
            }

            else
            {
                graphicsSystem.drawDebugOBB(ecsCoordinator.getComponent<TransformComponent>(entity), cameraSystem.getViewMatrix());
            }
        }
		else if (GLFWFunctions::debug_flag && ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
			graphicsSystem.drawDebugCircle(ecsCoordinator.getComponent<TransformComponent>(entity), cameraSystem.getViewMatrix());
		}
        if (isAnimate) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("bubbles 3.png"), transform.mdl_xform);
        }
        // Drawing based on entity components
        if (isEnemy) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("goldfish"), transform.mdl_xform);
        }
        else if (isPlayer) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("mossball"), transform.mdl_xform);
        }
        else if (isPump && !isAnimate) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("airVent"), transform.mdl_xform);

        }
        else if (isPlatform) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("woodtile"), transform.mdl_xform);
        }
        else if (isButton) {
            transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

            if (ecsCoordinator.getEntityID(entity) == "quitButton") {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("buttonQuit"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "retryButton")
            {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("buttonRetry"), transform.mdl_xform);
            }
        }
        else if (isCollectable) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("collectMoss"), transform.mdl_xform);
        }
        else if (isExit) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("exitFilter"), transform.mdl_xform);
        }
        else if (isBackground) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("background"), transform.mdl_xform);
        }
        else if (isUI) {
            transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

            if (GLFWFunctions::collectableCount == 0) {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("UI Counter-3"), transform.mdl_xform);
            }
            else if (GLFWFunctions::collectableCount == 1) {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("UI Counter-2"), transform.mdl_xform);
            }
            else if (GLFWFunctions::collectableCount == 2) {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("UI Counter-1"), transform.mdl_xform);
            }
            else if (GLFWFunctions::collectableCount >= 3) {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("UI Counter-0"), transform.mdl_xform);
            }
        }

        else if (ecsCoordinator.hasComponent<TransformComponent>(entity) &&
            ecsCoordinator.hasComponent<BehaviourComponent>(entity) &&
            ecsCoordinator.getEntitySignature(entity).count() == 2) {
           
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(ecsCoordinator.getEntityID(entity)), transform.mdl_xform);
        }

        std::cout << GLFWFunctions::collectableCount << std::endl;
    }
}


void GraphicSystemECS::cleanup() {}

std::string GraphicSystemECS::getSystemECS() {
    return "GraphicsSystemECS";
}