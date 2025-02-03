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
#include "GUIGameViewport.h"

#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include "Debug.h"
#include "GUIConsole.h"
#include "vector"


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

        bool hasAnimation = ecsCoordinator.hasComponent<AnimationComponent>(entity);
        AnimationComponent animation{};


        // Check if the entity has an animation component
        if (hasAnimation) {
            animation = ecsCoordinator.getComponent<AnimationComponent>(entity);
            if (animation.isAnimated) {
                animation.Update();
            }
        }

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

        // Use hasMovement for the update parameter
        //graphicsSystem.Update(dt / 10.0f, isAnimate || hasMovement || isEnemy);
       

        //graphicsSystem.Update(dt / 10.0f, (isAnimate&& isPump) || (isPlayer && hasMovement) || (isEnemy && hasMovement)); // Use hasMovement instead of true
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

        mouseBehaviour.update(entity);

        /*std::cout << GLFWFunctions::collectableCount << std::endl;*/
        /*--------------------------------------------------------------------------------
        --------------------------------------------------------------------------------*/

        // check if the player has collected all the collectables
        // Created a win text entity
        if (GLFWFunctions::collectableCount == 0 && GLFWFunctions::gameOver == false && GameViewWindow::getSceneNum() > -1) {
            createTextEntity(
                ecsCoordinator,
                "You Win!",
                "Antonio",
                myMath::Vector3D(1.0f, 1.0f, 1.0f), // White color
                myMath::Vector2D(-30, 40),         // Position
                "winTextBox"                       // Unique ID
            );
            GLFWFunctions::gameOver = true;
        }
        // lose text entity
        if (GLFWFunctions::instantLose && GLFWFunctions::gameOver == false) {
            createTextEntity(
                ecsCoordinator,
                "You Lose!",
                "Antonio",
                myMath::Vector3D(1.0f, 0.0f, 0.0f), // Red color
                myMath::Vector2D(-30, 40),          // Position
                "loseTextBox"                       // Unique ID
            );
            GLFWFunctions::gameOver = true;
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
            if (ecsCoordinator.getEntityID(entity) == "quitButton" || ecsCoordinator.getEntityID(entity) == "retryButton" || 
                ecsCoordinator.getEntityID(entity) == "startButton" || ecsCoordinator.getEntityID(entity) == "optionsButton" || 
                ecsCoordinator.getEntityID(entity) == "tutorialButton" || ecsCoordinator.getEntityID(entity) == "quitWindowButton" ||
                ecsCoordinator.getEntityID(entity) == "resumeButton" || ecsCoordinator.getEntityID(entity) == "closePauseMenu" ||
                ecsCoordinator.getEntityID(entity) == "pauseOptionsButton" || ecsCoordinator.getEntityID(entity) == "pauseTutorialButton" ||
                ecsCoordinator.getEntityID(entity) == "pauseQuitButton" || ecsCoordinator.getEntityID(entity) == "sfxSoundbar" ||
                ecsCoordinator.getEntityID(entity) == "musicSoundbar" || ecsCoordinator.getEntityID(entity) == "confirmButton" ||
                ecsCoordinator.getEntityID(entity) == "closeOptionsMenu")
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

        else if (ecsCoordinator.getEntityID(entity) == "gameLogo")
        {
            ecsCoordinator.setTextureID(entity, "gameLogo");
        }

        else if (ecsCoordinator.getEntityID(entity) == "pauseMenuBg")
        {
            transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
            ecsCoordinator.setTextureID(entity, "pauseMenu");
        }

        else if (ecsCoordinator.getEntityID(entity) == "optionsMenuBg")
        {
            transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
            ecsCoordinator.setTextureID(entity, "optionsMenu");
        }

        if (isUI) {
            transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

            if (GLFWFunctions::collectableCount == 0) {
                ecsCoordinator.setTextureID(entity, "UI Counter-3");
            }
            else if (GLFWFunctions::collectableCount == 1) {
                ecsCoordinator.setTextureID(entity, "UI Counter-2");
            }
            else if (GLFWFunctions::collectableCount == 2) {
                ecsCoordinator.setTextureID(entity, "UI Counter-1");
            }
            else if (GLFWFunctions::collectableCount >= 3) {
                ecsCoordinator.setTextureID(entity, "UI Counter-0");
            }
        }

        else if (isButton) {
            transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

            if (ecsCoordinator.getEntityID(entity) == "quitButton") {
                ecsCoordinator.setTextureID(entity, "buttonQuit");
            }

            else if (ecsCoordinator.getEntityID(entity) == "retryButton")
            {
                ecsCoordinator.setTextureID(entity, "buttonRetry");
            }

            else if (ecsCoordinator.getEntityID(entity) == "startButton")
            {
                if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
					ecsCoordinator.setTextureID(entity, "unactiveStartButton");
                else
					ecsCoordinator.setTextureID(entity, "activeStartButton");
            }

            else if (ecsCoordinator.getEntityID(entity) == "resumeButton")
            {
                if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
					ecsCoordinator.setTextureID(entity, "unactiveResumeButton");

                else
					ecsCoordinator.setTextureID(entity, "activeResumeButton");
            }

            else if (ecsCoordinator.getEntityID(entity) == "optionsButton" || ecsCoordinator.getEntityID(entity) == "pauseOptionsButton")
            {
                if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
					ecsCoordinator.setTextureID(entity, "unactiveOptionsButton");

                else
					ecsCoordinator.setTextureID(entity, "activeOptionsButton");
            }

            else if (ecsCoordinator.getEntityID(entity) == "tutorialButton" || ecsCoordinator.getEntityID(entity) == "pauseTutorialButton")
            {
                if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
					ecsCoordinator.setTextureID(entity, "unactiveTutorialButton");
                else
					ecsCoordinator.setTextureID(entity, "activeTutorialButton");
            }

            else if (ecsCoordinator.getEntityID(entity) == "confirmButton")
            {
                if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
					ecsCoordinator.setTextureID(entity, "unactiveConfirmButton");

                else
					ecsCoordinator.setTextureID(entity, "activeConfirmButton");
            }

            else if (ecsCoordinator.getEntityID(entity) == "quitWindowButton" || ecsCoordinator.getEntityID(entity) == "pauseQuitButton")
            {
                if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
					ecsCoordinator.setTextureID(entity, "unactiveQuitButton");
                else
					ecsCoordinator.setTextureID(entity, "activeQuitButton");
            }

            else if (ecsCoordinator.getEntityID(entity) == "closePauseMenu" || ecsCoordinator.getEntityID(entity) == "closeOptionsMenu")
            {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("closePopupButton"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "sfxSoundbar" || ecsCoordinator.getEntityID(entity) == "musicSoundbar")
            {
                std::string soundbarType = ecsCoordinator.getEntityID(entity);
                TransformComponent soundbarTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
                TransformComponent arrowTransform{};

                std::string audioArrowId = (soundbarType == "sfxSoundbar") ? "sfxSoundbarArrow" :
                                           (soundbarType == "musicSoundbar") ? "musicSoundbarArrow" : "";

                if (!audioArrowId.empty())
                {
                    for (auto& soundbarArrowEntity : ecsCoordinator.getAllLiveEntities())
                    {
                        if (ecsCoordinator.getEntityID(soundbarArrowEntity) == audioArrowId)
                        {
                            arrowTransform = ecsCoordinator.getComponent<TransformComponent>(soundbarArrowEntity);
                            break;
                        }
                    }

                    float soundbarLeftBoundary = soundbarTransform.position.GetX() - (soundbarTransform.scale.GetX() / 2.2f);

                    std::string textureName = (arrowTransform.position.GetX() <= soundbarLeftBoundary) ? "activeSoundbar" : "unactiveSoundbar";
                    graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(textureName), transform.mdl_xform);
                }


                //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveSoundbar"), transform.mdl_xform);
            }
        }
        else if (isCollectable) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("collectMoss"), transform.mdl_xform);
        }
        else if (isExit) {
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("exitFilter"), transform.mdl_xform);
        }
        else if (isBackground) {
            if (ecsCoordinator.getEntityID(entity) == "background")
            {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("background"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "mainMenuBg")
            {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("mainScreenBg"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "gameLogo")
            {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("gameLogo"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "pauseMenuBg")
            {
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("pauseMenu"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "optionsMenuBg")
            {
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("optionsMenu"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "sfxAudio" || ecsCoordinator.getEntityID(entity) == "musicAudio")
            {
                std::string audioType = ecsCoordinator.getEntityID(entity);
                TransformComponent arrowTransform{}, soundbarTransform{};

                std::string audioArrowId = (audioType == "sfxAudio") ? "sfxSoundbarArrow" :
                                           (audioType == "musicAudio") ? "musicSoundbarArrow" : "";

                std::string soundbarId = (audioType == "sfxAudio") ? "sfxSoundbar" :
                                         (audioType == "musicAudio") ? "musicSoundbar" : "";

                if (!audioArrowId.empty() && !soundbarId.empty())
                {
                    for (auto& soundbarEntity : ecsCoordinator.getAllLiveEntities())
                    {
                        if (ecsCoordinator.getEntityID(soundbarEntity) == soundbarId)
                        {
                            soundbarTransform = ecsCoordinator.getComponent<TransformComponent>(soundbarEntity);
                            break;
                        }
                    }

                    for (auto& arrowEntity : ecsCoordinator.getAllLiveEntities())
                    {
                        if (ecsCoordinator.getEntityID(arrowEntity) == audioArrowId)
                        {
                            arrowTransform = ecsCoordinator.getComponent<TransformComponent>(arrowEntity);
                            break;
                        }
                    }
                    
                    float soundbarLeftBoundary = soundbarTransform.position.GetX() - (soundbarTransform.scale.GetX() / 2.22f);

                    std::string textureName = (arrowTransform.position.GetX() <= soundbarLeftBoundary) ? "soundMute" : "soundOn";
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                    graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(textureName), transform.mdl_xform);
                }
            }

            else if (ecsCoordinator.getEntityID(entity) == "sfxSoundbarBase" || ecsCoordinator.getEntityID(entity) == "musicSoundbarBase")
            {
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("soundbarBase"), transform.mdl_xform);
            }

            else if (ecsCoordinator.getEntityID(entity) == "sfxSoundbarArrow" || ecsCoordinator.getEntityID(entity) == "musicSoundbarArrow")
            {
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("soundbarArrow"), transform.mdl_xform);
            }
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

            if (ecsCoordinator.getTextureID(entity) != "") {
                //if (entityAnimate) {
                    graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(ecsCoordinator.getTextureID(entity)), transform.mdl_xform, animation.currentUVs);
				//}
                //else {
                //    std::vector<glm::vec2> UVs{
                //        glm::vec2(1.0f, 1.0f),
                //        glm::vec2(1.0f, 0.0f),
                //        glm::vec2(0.0f, 0.0f),
                //        glm::vec2(0.0f, 1.0f)
                //    };
                //    graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(ecsCoordinator.getTextureID(entity)), transform.mdl_xform, UVs);
                //}
            }
        }
    }


void GraphicSystemECS::cleanup() {}

std::string GraphicSystemECS::getSystemECS() {
    return "GraphicsSystemECS";
}