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

bool gameover = false;

void createTextEntity(
    ECSCoordinator& ecs,
    const std::string& text,
    const std::string& fontId,
    const myMath::Vector3D& color,
    const myMath::Vector2D& position,
    const std::string& entityId
) {
    Entity textEntity = ecs.createEntity();

    // Font Component
    FontComponent font{};
    font.text = text;
    font.textScale = 1.0f;
    font.color = color;
    font.fontId = fontId;
    ecsCoordinator.addComponent(textEntity, font);

    // Transform Component
    TransformComponent transform{};
    transform.position = position;
    transform.scale.SetX(0); // Initial scale (could be animated later)
    transform.scale.SetY(0);
    ecsCoordinator.addComponent(textEntity, transform);

    // Behaviour Component
    BehaviourComponent behaviour{};
    behaviour.none = true;
    ecsCoordinator.addComponent(textEntity, behaviour);

    // Assign an ID for reference
    ecsCoordinator.setEntityID(textEntity, entityId);

    // Debugging log (optional)
    // std::cout << entityId << " Text Created" << std::endl;
}

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
        bool isAnimated = false; // Default is false
        if (ecsCoordinator.hasComponent<AnimationComponent>(entity)) {
            auto& animationComponent = ecsCoordinator.getComponent<AnimationComponent>(entity);
            isAnimated = animationComponent.isAnimated;

            // Initialize animation if it hasn't been initialized yet
            if (isAnimated && !graphicsSystem.HasAnimationData(entity)) {
                graphicsSystem.InitializeAnimation(entity, animationComponent);
            }
        }
        Console::GetLog() << "Entity: " << entity << " Animation: " << (isAnimated ? "True" : "False") << std::endl;

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
        bool shouldAnimate = false;  // Start with false

        // For pump components
        if (ecsCoordinator.hasComponent<PumpComponent>(entity)) {
            const auto& pumpComponent = ecsCoordinator.getComponent<PumpComponent>(entity);
            shouldAnimate = pumpComponent.isAnimate && pumpComponent.isPump;
        }
        // For regular animated entities (player/enemy)
        else if (isAnimated) {  // Only check these if the base isAnimated is true
            if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
                shouldAnimate = ecsCoordinator.hasComponent<PhysicsComponent>(entity);
            }
            else if (ecsCoordinator.hasComponent<EnemyComponent>(entity)) {
                shouldAnimate = ecsCoordinator.hasComponent<PhysicsComponent>(entity);
            }
        }
        // Use hasMovement for the update parameter
        if (shouldAnimate) {
            graphicsSystem.UpdateAnimation(entity, dt , true);
        }
        else {
            // Set default UV coordinates for non-animated entities
            graphicsSystem.UpdateAnimation(entity, dt , false);
        }
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

        /*std::cout << GLFWFunctions::collectableCount << std::endl;*/
        /*--------------------------------------------------------------------------------
        --------------------------------------------------------------------------------*/

		// check if the player has collected all the collectables
		// Created a win text entity
        if (GLFWFunctions::collectableCount == 0 && gameover == false) {
            createTextEntity(
                ecsCoordinator,
                "You Win!",
                "Antonio",
                myMath::Vector3D(1.0f, 1.0f, 1.0f), // White color
                myMath::Vector2D(-30, 40),         // Position
                "winTextBox"                       // Unique ID
            );
            gameover = true;
        }
		// lose text entity
        if (GLFWFunctions::instantLose && gameover == false) {
            createTextEntity(
                ecsCoordinator,
                "You Lose!",
                "Antonio",
                myMath::Vector3D(1.0f, 0.0f, 0.0f), // Red color
                myMath::Vector2D(-30, 40),          // Position
                "loseTextBox"                       // Unique ID
            );
            gameover = true;
        }
        // 
        if (GLFWFunctions::collectableCount == 0 && GLFWFunctions::exitCollision) {
            if (ecsCoordinator.getEntityID(entity) == "winTextBox")
            {
                auto& font = ecsCoordinator.getComponent<FontComponent>(entity);
                font.text = "Exit!";
            }
        }
        // cheat code 
		if (GLFWFunctions::instantWin)
		{
			GLFWFunctions::collectableCount = 0;
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
        if (isAnimate && GLFWFunctions::isPumpOn) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("bubbles 3.png"), transform.mdl_xform);
        }
        // Drawing based on entity components
        if (isEnemy) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("goldfish"), transform.mdl_xform);
        }
        else if (isPlayer && isAnimated) {
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
                 ecsCoordinator.getEntitySignature(entity).count() == 3) {
                 graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(ecsCoordinator.getEntityID(entity)), transform.mdl_xform);
       }
    }
}


void GraphicSystemECS::cleanup() {}

std::string GraphicSystemECS::getSystemECS() {
    return "GraphicsSystemECS";
}