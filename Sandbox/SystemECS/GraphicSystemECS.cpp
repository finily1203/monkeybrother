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

         Javier Chua (javierjunliang.chua): Handle the logic of rendering of the animation
*//*___________________________________________________________________________-*/
#include "GraphicSystemECS.h"

#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
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


//std::unique_ptr<EntityManager> entityManager;
//Initialise currently does not do anything
void GraphicSystemECS::initialise() {

}

double elapsedTimeSinceGrowStart(const PlayerComponent& player) {
    if (!player.isGrowing) return 1.0f;
    return glfwGetTime() - player.growStartTime;
}



void GraphicSystemECS::handlePlayerMovementAnimation(Entity playerEntity, TransformComponent& transform, AnimationComponent& animation, float velocityMagnitude) {
    // Check for player death first
    if (GLFWFunctions::isPlayerDead) {

        static bool deathAnimationStarted = false;
        static float deathAnimationTime = 0.0f;
        static int deathCurrentFrame = 0;

        if (!deathAnimationStarted) {

            deathAnimationStarted = true;
            deathAnimationTime = 0.0f;
            deathCurrentFrame = 0;
            ecsCoordinator.setTextureID(playerEntity, "mossballDead");
        }


        animation.totalFrames = 24;
        animation.columns = 8;
        animation.rows = 3;


        deathAnimationTime += GLFWFunctions::delta_time;
        float frameDuration = 0.1f;

        if (deathAnimationTime >= frameDuration && deathCurrentFrame < static_cast<int>(animation.totalFrames) - 1) {
            deathCurrentFrame++;
            deathAnimationTime = 0.0f;
        }


        animation.currentFrame = deathCurrentFrame;
        animation.UpdateUVCoordinates();

        // Draw the death animation
        graphicsSystem.DrawObject(
            GraphicsSystem::DrawMode::TEXTURE,
            assetsManager.GetTexture("mossballDead"),
            transform.mdl_xform,
            animation.currentUVs
        );
        return;
    }
    else {
        // Reset death animation state when player is alive
        static bool deathAnimationStarted = false;
        if (deathAnimationStarted) {
            deathAnimationStarted = false;
        }
    }

    const MovementAnimConfig& config = animation.movementConfig;

    if (config.bodyTexture.empty() || config.eyesTexture.empty() ||
        config.bodyFrames == 0 || config.eyesFrames == 0) {
        // Use default animation
        std::string defaultTexture = ecsCoordinator.getTextureID(playerEntity);
        graphicsSystem.DrawObject(
            GraphicsSystem::DrawMode::TEXTURE,
            assetsManager.GetTexture(defaultTexture),
            transform.mdl_xform,
            animation.currentUVs
        );
        return;
    }

    auto& player = ecsCoordinator.getComponent<PlayerComponent>(playerEntity);
    static float eyesAnimTime = 0.0f;
    static int eyesCurrentFrame = 0;

    if (config.movementThreshold > 0 && velocityMagnitude > config.movementThreshold) {
        player.lastMoveTime = glfwGetTime();
        player.isIdle = false;
        // Body animation
        AnimationComponent bodyAnimation = animation;
        bodyAnimation.isAnimated = true;
        bodyAnimation.totalFrames = config.bodyFrames;
        bodyAnimation.columns = config.bodyColumns;
        bodyAnimation.rows = config.bodyRows;
        bodyAnimation.UpdateUVCoordinates();
        graphicsSystem.DrawObject(
            GraphicsSystem::DrawMode::TEXTURE,
            assetsManager.GetTexture(config.bodyTexture),
            transform.mdl_xform,
            bodyAnimation.currentUVs
        );

        if (config.eyeFrameDuration > 0) {
            eyesAnimTime += GLFWFunctions::delta_time;
            eyesCurrentFrame = static_cast<int>(eyesAnimTime / config.eyeFrameDuration)
                % static_cast<int>(config.eyesFrames);
            AnimationComponent eyesAnimation = animation;
            eyesAnimation.isAnimated = true;
            eyesAnimation.totalFrames = config.eyesFrames;
            eyesAnimation.columns = config.eyesColumns;
            eyesAnimation.rows = config.eyesRows;
            eyesAnimation.currentFrame = eyesCurrentFrame;
            eyesAnimation.UpdateUVCoordinates();
            graphicsSystem.DrawObject(
                GraphicsSystem::DrawMode::TEXTURE,
                assetsManager.GetTexture(config.eyesTexture),
                transform.mdl_xform,
                eyesAnimation.currentUVs
            );
        }
    }
    else {
        std::string defaultTexture = ecsCoordinator.getTextureID(playerEntity);
        graphicsSystem.DrawObject(
            GraphicsSystem::DrawMode::TEXTURE,
            assetsManager.GetTexture(defaultTexture),
            transform.mdl_xform,
            animation.currentUVs
        );
        // Reset eye animation
        eyesAnimTime = 0.0f;
        eyesCurrentFrame = 0;
        // Check for idle animation
        double currentTime = glfwGetTime();
        const float IDLE_THRESHOLD = 3.0f;
        if (!player.isIdle && !player.playingIdleAnim &&
            (currentTime - player.lastMoveTime > IDLE_THRESHOLD)) {
            player.isIdle = true;
            player.playingIdleAnim = true;
            player.idleAnimStart = currentTime;
        }
    }
}

void GraphicSystemECS::handlePlayerGrowthAnimation(Entity playerEntity, TransformComponent& transform, AnimationComponent& animation) {
    auto& player = ecsCoordinator.getComponent<PlayerComponent>(playerEntity);

    // Get the growth configuration
    const ComplexAnimationConfig& growthConfig = animation.growthConfig;

    // skip custom animation if not loaded properly
    if (growthConfig.body.textureName.empty() || growthConfig.eyes.textureName.empty() ||
        growthConfig.duration <= 0 || growthConfig.body.totalFrames <= 0) {
        return;
    }

    if (player.isGrowing) {
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - player.growStartTime;

        if (elapsedTime <= growthConfig.duration) {
            // Store original animation state
            bool originalIsAnimated = animation.isAnimated;
            int originalCurrentFrame = animation.currentFrame;
            float originalColumns = animation.columns;
            float originalRows = animation.rows;
            std::vector<glm::vec2> originalUVs = animation.currentUVs;

            // Calculate frame progress
            float frameProgress = static_cast<float>(elapsedTime / growthConfig.duration);
            int currentGrowthFrame = static_cast<int>(frameProgress * growthConfig.body.totalFrames);
            if (currentGrowthFrame >= static_cast<int>(growthConfig.body.totalFrames)) {
                currentGrowthFrame = static_cast<int>(growthConfig.body.totalFrames) - 1;
            }


            animation.isAnimated = true;
            animation.currentFrame = currentGrowthFrame;
            animation.columns = growthConfig.body.columns;
            animation.rows = growthConfig.body.rows;
            animation.totalFrames = growthConfig.body.totalFrames;
            animation.UpdateUVCoordinates();


            std::string originalTexture = ecsCoordinator.getTextureID(playerEntity);

            // Draw body animation
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE,
                assetsManager.GetTexture(growthConfig.body.textureName),
                transform.mdl_xform, animation.currentUVs);

            // Draw eye animation if configured
            if (!growthConfig.eyes.textureName.empty() && growthConfig.eyes.totalFrames > 0) {
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE,
                    assetsManager.GetTexture(growthConfig.eyes.textureName),
                    transform.mdl_xform, animation.currentUVs);
            }

            // Restore original animation state
            animation.isAnimated = originalIsAnimated;
            animation.currentFrame = originalCurrentFrame;
            animation.columns = originalColumns;
            animation.rows = originalRows;
            animation.currentUVs = originalUVs;
            ecsCoordinator.setTextureID(playerEntity, originalTexture);
        }
        else {

            player.isGrowing = false;

            // Reset animation state
            animation.currentTime = 0.0f;
            animation.Update();
        }
    }
}

void GraphicSystemECS::handlePlayerIdleAnimation(Entity playerEntity, TransformComponent& transform, AnimationComponent& animation) {
    auto& player = ecsCoordinator.getComponent<PlayerComponent>(playerEntity);

    // Get the idle configuration
    const ComplexAnimationConfig& idleConfig = animation.idleConfig;


    if (idleConfig.body.textureName.empty() || idleConfig.duration <= 0 ||
        idleConfig.body.totalFrames <= 0) {
        return;
    }

    if (player.playingIdleAnim) {
        double currentTime = glfwGetTime();
        double elapsedIdleTime = currentTime - player.idleAnimStart;

        // Store original animation state
        bool originalIsAnimated = animation.isAnimated;
        int originalCurrentFrame = animation.currentFrame;
        float originalColumns = animation.columns;
        float originalRows = animation.rows;
        std::vector<glm::vec2> originalUVs = animation.currentUVs;

        // Calculate frame progress
        float frameProgress = static_cast<float>(elapsedIdleTime / idleConfig.duration);
        if (frameProgress > 1.0f) {
            frameProgress = 1.0f;

            if (elapsedIdleTime > idleConfig.duration + 0.1f) {
                player.playingIdleAnim = false;
            }
        }

        // Calculate current body frame
        int currentBodyFrame = static_cast<int>(frameProgress * idleConfig.body.totalFrames);
        if (currentBodyFrame >= static_cast<int>(idleConfig.body.totalFrames)) {
            currentBodyFrame = static_cast<int>(idleConfig.body.totalFrames) - 1;
        }

        // Draw body animation
        animation.isAnimated = true;
        animation.currentFrame = currentBodyFrame;
        animation.columns = idleConfig.body.columns;
        animation.rows = idleConfig.body.rows;
        animation.totalFrames = idleConfig.body.totalFrames;
        animation.UpdateUVCoordinates();
        graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE,
            assetsManager.GetTexture(idleConfig.body.textureName),
            transform.mdl_xform, animation.currentUVs);

        // Draw eye animation if configured
        if (!idleConfig.eyes.textureName.empty() && idleConfig.eyes.totalFrames > 0) {
            // Calculate current eye frame
            int currentEyesFrame = static_cast<int>(frameProgress * idleConfig.eyes.totalFrames);
            if (currentEyesFrame >= static_cast<int>(idleConfig.eyes.totalFrames)) {
                currentEyesFrame = static_cast<int>(idleConfig.eyes.totalFrames) - 1;
            }

            animation.currentFrame = currentEyesFrame;
            animation.columns = idleConfig.eyes.columns;
            animation.rows = idleConfig.eyes.rows;
            animation.totalFrames = idleConfig.eyes.totalFrames;
            animation.UpdateUVCoordinates();
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE,
                assetsManager.GetTexture(idleConfig.eyes.textureName),
                transform.mdl_xform, animation.currentUVs);
        }

        // Restore original animation state
        animation.isAnimated = originalIsAnimated;
        animation.currentFrame = originalCurrentFrame;
        animation.columns = originalColumns;
        animation.rows = originalRows;
        animation.currentUVs = originalUVs;
    }
}

//Update function to update the graphics system
//uses functions from GraphicsSystem class to update, draw
//and render objects.
void GraphicSystemECS::update(float dt) {
    (void)dt;

    // Cache player entity and transform once at start of frame
    Entity playerEntity = Entity{};
    TransformComponent* playerTransform = nullptr;


    Entity fpsEntity = ecsCoordinator.getFPSDisplayEntity();
    if (fpsEntity != 0 && ecsCoordinator.hasComponent<FontComponent>(fpsEntity)) {
        auto& font = ecsCoordinator.getComponent<FontComponent>(fpsEntity);
        font.text = GLFWFunctions::showFPS ? ("FPS:" + std::to_string(static_cast<int>(GLFWFunctions::fps))) : "";
    }


    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
            playerEntity = entity;
            playerTransform = &ecsCoordinator.getComponent<TransformComponent>(entity);
            break;
        }
    }


    if (GLFWFunctions::allow_camera_movement) {
        cameraSystem.update();
    }
    else if (playerTransform != nullptr) {
        cameraSystem.lockToComponent(*playerTransform);
        cameraSystem.update();
    }


    myMath::Matrix3x3 identityMatrix = { 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f };


    if (GLFWFunctions::instantWin) {
        GLFWFunctions::collectableCount = 0;
    }


    bool doLogging = false;

    // Cache view matrix - don't recalculate per entity
    myMath::Matrix3x3 viewMatrix = cameraSystem.getViewMatrix();

    // Cache and preprocess audio UI components
    std::map<std::string, TransformComponent> arrowTransforms;
    std::vector<std::pair<Entity, TransformComponent>> sfxNotches, musicNotches;

    // Pre-process audio component data outside the main rendering loop
    if (GLFWFunctions::gamePaused || GameViewWindow::getSceneNum() == -1) {
        for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
            std::string entityId = ecsCoordinator.getEntityID(entity);


            if (entityId == "sfxSoundbarArrow" || entityId == "musicSoundbarArrow") {
                arrowTransforms[entityId] = ecsCoordinator.getComponent<TransformComponent>(entity);
            }

            // Cache audio notches
            if (entityId.find("sfxNotch") != std::string::npos) {
                sfxNotches.emplace_back(entity, ecsCoordinator.getComponent<TransformComponent>(entity));
            }
            else if (entityId.find("musicNotch") != std::string::npos) {
                musicNotches.emplace_back(entity, ecsCoordinator.getComponent<TransformComponent>(entity));
            }
        }
    }

    // Render entities layer by layer
    for (int layerIndex = 0; layerIndex < layerManager.getLayerCount(); layerIndex++) {
        if (!layerManager.getLayerVisibility(layerIndex)) {
            continue; // Skip invisible layers entirely
        }

        for (auto entity : layerManager.getEntitiesFromLayer(layerIndex)) {
            auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
            
            if (doLogging) {
                Console::GetLog() << "Entity: " << entity << " Position: " << transform.position.GetX() << ", " << transform.position.GetY() << std::endl;
            }

            
            std::string entityId = ecsCoordinator.getEntityID(entity);

            // Handle animation component
            AnimationComponent animation{};
            bool hasAnimation = ecsCoordinator.hasComponent<AnimationComponent>(entity);

            if (hasAnimation) {
                animation = ecsCoordinator.getComponent<AnimationComponent>(entity);

                if (GameViewWindow::getPaused() || GLFWFunctions::gamePaused) {
                    animation.UpdateUVCoordinates(); // Just update UVs without advancing frame
                }
                else {
                    animation.isAnimated = true;
                    animation.Update();
                }
            }

            // Get common component flags only once
            bool isPlayer = ecsCoordinator.hasComponent<PlayerComponent>(entity);
            bool isButton = ecsCoordinator.hasComponent<ButtonComponent>(entity);
            bool isUI = ecsCoordinator.hasComponent<UIComponent>(entity);
            bool isFilter = ecsCoordinator.hasComponent<FilterComponent>(entity);
            bool isEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);
            bool isNavigation = ecsCoordinator.hasComponent<NavigationComponent>(entity);


            auto& behaviour = ecsCoordinator.getComponent<BehaviourComponent>(entity);

            // Update model transformation
            bool isUIElement = isButton || isUI ||
                entityId == "pauseMenuBg" ||
                entityId == "optionsMenuBg" ||
                entityId == "tutorialBaseBg" ||
                entityId == "pageCounter" ||
                entityId == "rotationSpeedSliderNotch" ||
                entityId == "quitLevelMenuBase" ||
                entityId == "levelCompletedMenuBase" ||
                entityId == "gameOverBG" ||
                entityId.find("Soundbar") != std::string::npos ||
                entityId.find("sfxNotch") != std::string::npos ||
                entityId.find("musicNotch") != std::string::npos;

            if (isUIElement) {
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
            }
            else if (isNavigation && entityId == "nav_arrow") {
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, viewMatrix);
            }
            else {
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, viewMatrix);
            }

            // Update mouse behavior for interactive elements
            if (isButton) {
                mouseBehaviour.update(entity);
            }

            if (isNavigation) {
                auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(entity);
                if (!navComp.isVisible) {
                    continue;
                }
            }


            if (GLFWFunctions::debug_flag) {
                if (!ecsCoordinator.hasComponent<FontComponent>(entity) && !isPlayer) {
                    graphicsSystem.drawDebugOBB(transform, isUIElement ? identityMatrix : viewMatrix);
                }
                else if (isPlayer) {
                    graphicsSystem.drawDebugCircle(transform, viewMatrix);
                }
            }


            if (entityId == "cutsceneBg") {
                ecsCoordinator.setTextureID(entity, "cutsceneBackground");
                mouseBehaviour.update(entity);
            }
            // Game logo
            else if (entityId == "gameLogo") {
                ecsCoordinator.setTextureID(entity, "gameLogo");
            }
            // Pause menu background
            else if (entityId == "pauseMenuBg") {
                ecsCoordinator.setTextureID(entity, "pauseMenu");
            }
            // Options menu background
            else if (entityId == "optionsMenuBg") {
                ecsCoordinator.setTextureID(entity, "optionsMenu");
            }
            // Tutorial page
            else if (entityId == "tutorialBaseBg") {
                const std::vector<std::string> textureIds = {
                    "tutorialControlsBase", "tutorialMovementBase",
                    "tutorialKeyItems1Base", "tutorialKeyItems2Base",
                    "tutorialWaterCurrentBase", "tutorialEscapeBase",
                    "tutorialFilterBase", "tutorialFishBase"
                };

                int currentPage = GLFWFunctions::tutorialCurrentPage;
                if (currentPage >= 1 && currentPage <= static_cast<int>(textureIds.size())) {
                    ecsCoordinator.setTextureID(entity, textureIds[currentPage - 1]);
                }
            }

            else if (entityId == "pageCounter") {
                const std::vector<std::string> textureIds = {
                    "pageCounter1", "pageCounter2", "pageCounter3",
                    "pageCounter4", "pageCounter5", "pageCounter6",
                    "pageCounter7", "pageCounter8"
                };

                int currentPage = GLFWFunctions::tutorialCurrentPage;
                if (currentPage >= 1 && currentPage <= static_cast<int>(textureIds.size())) {
                    ecsCoordinator.setTextureID(entity, textureIds[currentPage - 1]);
                }
            }

            else if (entityId == "rotationSpeedSliderNotch") {
                ecsCoordinator.setTextureID(entity, "rotationSpeedNotch");
            }

            else if (entityId == "quitLevelMenuBase") {
                ecsCoordinator.setTextureID(entity, "quitLevelBase");
            }

            else if (entityId == "levelCompletedMenuBase") {
                ecsCoordinator.setTextureID(entity, "levelCompletedBase");
            }

            else if (entityId == "gameOverBG") {
                ecsCoordinator.setTextureID(entity, "GameOverBG");
            }


            if (entityId.find("sfxNotch") != std::string::npos || entityId.find("musicNotch") != std::string::npos) {

                continue;
            }


            if (entityId == "sfxSoundbarArrow" || entityId == "musicSoundbarArrow") {
                ecsCoordinator.setTextureID(entity, "soundbarArrow");
            }


            if (isUI) {
                if (GLFWFunctions::collectableCount == 0) {
                    ecsCoordinator.setTextureID(entity, "UI Counter-3");
                }
                else if (GLFWFunctions::collectableCount == 1) {
                    ecsCoordinator.setTextureID(entity, "UI Counter-2");
                }
                else if (GLFWFunctions::collectableCount == 2) {
                    ecsCoordinator.setTextureID(entity, "UI Counter-1");
                }
                else {
                    ecsCoordinator.setTextureID(entity, "UI Counter-0");
                }
            }


            if (behaviour.exit) {
                if (GLFWFunctions::collectableCount == 0) {
                    ecsCoordinator.setTextureID(entity, "cloggedVent3");
                }
                else if (GLFWFunctions::collectableCount == 1) {
                    ecsCoordinator.setTextureID(entity, "cloggedVent2");
                }
                else if (GLFWFunctions::collectableCount == 2) {
                    ecsCoordinator.setTextureID(entity, "cloggedVent1");
                }
                else {
                    ecsCoordinator.setTextureID(entity, "cloggedVent0");
                }
            }


            if (isButton) {
                std::string hoveredButton = mouseBehaviour.getHoveredButton();
               

                if (entityId == "quitButton") {
                    ecsCoordinator.setTextureID(entity, "buttonQuit");
                }
                else if (entityId == "retryButton") {
                    ecsCoordinator.setTextureID(entity, "buttonRetry");
                }
                else if (entityId == "pauseRetryButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveRetryButton" : "activeRetryButton");
                }
                else if (entityId == "startButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveStartButton" : "activeStartButton");
                }
                else if (entityId == "resumeButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveResumeButton" : "activeResumeButton");
                }
                else if (entityId == "optionsButton" || entityId == "pauseOptionsButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveOptionsButton" : "activeOptionsButton");
                }
                else if (entityId == "tutorialButton" || entityId == "pauseTutorialButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveTutorialButton" : "activeTutorialButton");
                }
                else if (entityId == "tutorialClick") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "tutorial_inactive" : "tutorial_active");
                }
                else if (entityId == "confirmButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveConfirmButton" : "activeConfirmButton");
                }
                else if (entityId == "quitWindowButton" || entityId == "pauseQuitButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveQuitButton" : "activeQuitButton");
                }
                else if (entityId == "quitToMainMenuButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveYesButton" : "activeYesButton");
                }
                else if (entityId == "returnToPauseMenuButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveNoButton" : "activeNoButton");
                }
                else if (entityId == "nextLevelButton") {
                    updateButtons();
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveNextLevelButton" : "activeNextLevelButton");
                }
                else if (entityId == "mainMenuButton") {
                    ecsCoordinator.setTextureID(entity, entityId != hoveredButton ? "unactiveMainMenuButton" : "activeMainMenuButton");
                }
                else if (entityId == "closePauseMenu" || entityId == "closeOptionsMenu" || entityId == "closeTutorialMenu") {
                    ecsCoordinator.setTextureID(entity, "closePopupButton");
                }
                else if (entityId == "sfxSoundbarBase" || entityId == "musicSoundbarBase") {
                    ecsCoordinator.setTextureID(entity, "soundbarBase");
                }
                else if (entityId == "nextTutorialPage") {
                    ecsCoordinator.setTextureID(entity, "rightArrow");
                    updateTutorialArrows();
                }
                else if (entityId == "previousTutorialPage") {
                    ecsCoordinator.setTextureID(entity, "leftArrow");
                    updateTutorialArrows();
                }
                else if (entityId == "rotationSpeedSlider") {
                    ecsCoordinator.setTextureID(entity, "rotationSpeedSlider");
                }
                else if (entityId == "gameOverRetryButton") {
                    ecsCoordinator.setTextureID(entity, "buttonRetry");
                }
                else if (entityId == "gameOverQuitButton") {
                    ecsCoordinator.setTextureID(entity, "buttonQuit");
                }
            }


            if (isPlayer) {
                auto& player = ecsCoordinator.getComponent<PlayerComponent>(entity);

                // Skip invisible players
                if (!player.isVisible) {
                    continue;
                }

                // Handle player animations with priorities
                if (player.playingIdleAnim) {
                    handlePlayerIdleAnimation(entity, transform, animation);
                    continue;
                }

                if (player.isGrowing) {
                    handlePlayerGrowthAnimation(entity, transform, animation);
                    const float GROWTH_DURATION = 1.0f;
                    if (elapsedTimeSinceGrowStart(player) < GROWTH_DURATION) {
                        continue;
                    }
                }

                if (!ecsCoordinator.hasComponent<PhysicsComponent>(entity)) {
                    continue;
                }

                auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
                float velocityMagnitude = std::hypot(physics.velocity.GetX(), physics.velocity.GetY());

                handlePlayerMovementAnimation(entity, transform, animation, velocityMagnitude);
                continue;
            }

            // Handle filter state
            if (isFilter) {
                auto& filter = ecsCoordinator.getComponent<FilterComponent>(entity);
                ecsCoordinator.setTextureID(entity, filter.isFilterClogged ? "filter_mossed" : "exitFilter");
            }


            if (isEnemy) {
                auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);


                if (enemy.currState == 0) {
                    ecsCoordinator.setTextureID(entity, "goldfish");
                    if (hasAnimation) {
                        auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                        enemyAnimation.totalFrames = 24;
                        enemyAnimation.columns = 4;
                        enemyAnimation.rows = 6;
                    }
                }
                else if (enemy.currState == 2) {
                    ecsCoordinator.setTextureID(entity, "goldfishBite");
                    if (hasAnimation) {
                        auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                        enemyAnimation.totalFrames = 16;
                        enemyAnimation.columns = 4;
                        enemyAnimation.rows = 4;
                    }
                }


                if (ecsCoordinator.getTextureID(entity) == "goldfishAlert" && hasAnimation) {
                    auto& anim = ecsCoordinator.getComponent<AnimationComponent>(entity);
                    double currentAbsoluteTime = glfwGetTime();
                    double timeSinceCreation = currentAbsoluteTime - anim.creationTime;
                    anim.currentFrame = static_cast<int>((timeSinceCreation / anim.frameTime)) % static_cast<int>(anim.totalFrames);

                    if (anim.currentFrame == static_cast<int>(anim.totalFrames) - 1) {
                        ecsCoordinator.setTextureID(entity, "goldfish");
                        auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                        enemyAnimation.totalFrames = 24;
                        enemyAnimation.frameTime = 0.05f;
                        enemyAnimation.columns = 4;
                        enemyAnimation.rows = 6;
                    }
                }
            }

            // Handle temporary animation entities
            if (entityId == "collectAnimation" || entityId == "filterPush" ||
                entityId == "fishAlertAnimation" || entityId == "fishAttackAnimation") {

                if (hasAnimation) {
                    auto& anim = ecsCoordinator.getComponent<AnimationComponent>(entity);
                    double currentAbsoluteTime = glfwGetTime();
                    double timeSinceCreation = currentAbsoluteTime - anim.creationTime;
                    anim.currentFrame = static_cast<int>((timeSinceCreation / anim.frameTime)) % static_cast<int>(anim.totalFrames);

                    if (anim.currentFrame == static_cast<int>(anim.totalFrames) - 1) {
                        ecsCoordinator.destroyEntity(entity);
                        continue;
                    }
                }
            }


            std::string textureId = ecsCoordinator.getTextureID(entity);
            if (!textureId.empty()) {

                if (textureId == "filter_in.png" && GLFWFunctions::filterClogged) {
                    continue;
                }


                if (textureId == "bubbles 3.png" && !GLFWFunctions::isPumpOn) {
                    continue;
                }

                graphicsSystem.DrawObject(
                    GraphicsSystem::DrawMode::TEXTURE,
                    assetsManager.GetTexture(textureId),
                    transform.mdl_xform,
                    animation.currentUVs
                );
            }
        }
    }


    if ((GLFWFunctions::gamePaused || GameViewWindow::getSceneNum() == -1) &&
        !sfxNotches.empty() && !musicNotches.empty() &&
        arrowTransforms.count("sfxSoundbarArrow") && arrowTransforms.count("musicSoundbarArrow")) {

        TransformComponent& sfxArrowTransform = arrowTransforms["sfxSoundbarArrow"];
        TransformComponent& musicArrowTransform = arrowTransforms["musicSoundbarArrow"];


        int activeNotchesSFX = 0, activeNotchesMusic = 0;

        // Process SFX notches
        if (sfxNotches.size() == 10) {
            float startPosSFX = sfxNotches[0].second.position.GetX() - (sfxNotches[0].second.scale.GetX() / 2.0f);
            float endPosSFX = sfxNotches[9].second.position.GetX() + (sfxNotches[9].second.scale.GetX() / 2.0f);
            float arrowPosSFX = sfxArrowTransform.position.GetX();
            float progressSFX = std::abs((arrowPosSFX - startPosSFX) / (endPosSFX - startPosSFX));
            float sfxPercentage = std::round(progressSFX * 10.f) * 10.f;
            sfxPercentage = std::clamp(sfxPercentage, 0.f, 100.f);
            AudioSystem::sfxPercentage = sfxPercentage;

            // Calculate active notches
            for (size_t j = 0; j < sfxNotches.size(); ++j) {
                if (sfxArrowTransform.position.GetX() + sfxArrowTransform.scale.GetX() / 2.35f >= sfxNotches[j].second.position.GetX()) {
                    activeNotchesSFX = static_cast<int>(j) + 1;
                }
            }

            // Draw SFX notches
            for (size_t j = 0; j < sfxNotches.size(); ++j) {
                std::string notchTexture = (j < activeNotchesSFX) ? "activeSoundbarNotch" : "unactiveSoundbarNotch";
                TransformComponent& notchTransform = sfxNotches[j].second;
                notchTransform.mdl_xform = graphicsSystem.UpdateObject(notchTransform.position, notchTransform.scale, notchTransform.orientation, identityMatrix);
                AnimationComponent dummyAnim;
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(notchTexture), notchTransform.mdl_xform, dummyAnim.currentUVs);
            }
        }

        // Process Music notches
        if (musicNotches.size() == 10) {
            float startPosMusic = musicNotches[0].second.position.GetX() - (musicNotches[0].second.scale.GetX() / 2.0f);
            float endPosMusic = musicNotches[9].second.position.GetX() + (musicNotches[9].second.scale.GetX() / 2.0f);
            float arrowPosMusic = musicArrowTransform.position.GetX();
            float progressMusic = std::abs((arrowPosMusic - startPosMusic) / (endPosMusic - startPosMusic));
            float musicPercentage = std::round(progressMusic * 10.f) * 10.f;
            musicPercentage = std::clamp(musicPercentage, 0.f, 100.f);
            AudioSystem::musicPercentage = musicPercentage;

            // Calculate active notches
            for (size_t j = 0; j < musicNotches.size(); ++j) {
                if (musicArrowTransform.position.GetX() + musicArrowTransform.scale.GetX() / 2.35f >= musicNotches[j].second.position.GetX()) {
                    activeNotchesMusic = static_cast<int>(j) + 1;
                }
            }

            // Draw Music notches
            for (size_t j = 0; j < musicNotches.size(); ++j) {
                std::string notchTexture = (j < activeNotchesMusic) ? "activeSoundbarNotch" : "unactiveSoundbarNotch";
                TransformComponent& notchTransform = musicNotches[j].second;
                notchTransform.mdl_xform = graphicsSystem.UpdateObject(notchTransform.position, notchTransform.scale, notchTransform.orientation, identityMatrix);
                AnimationComponent dummyAnim;
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(notchTexture), notchTransform.mdl_xform, dummyAnim.currentUVs);
            }
        }
    }
}

// this is to update the left and right arrows scale in real-time 
//ADDDED TUTORIAL BUTTON TO THIS TOO @IAN
void GraphicSystemECS::updateTutorialArrows()
{
    Entity nextArrow = ecsCoordinator.getEntityFromID("nextTutorialPage");
    Entity previousArrow = ecsCoordinator.getEntityFromID("previousTutorialPage");
	Entity tutorialButton = ecsCoordinator.getEntityFromID("tutorialClick");

    TransformComponent& nextTransform = ecsCoordinator.getComponent<TransformComponent>(nextArrow);
    TransformComponent& previousTransform = ecsCoordinator.getComponent<TransformComponent>(previousArrow);
	TransformComponent& tutorialTransform = ecsCoordinator.getComponent<TransformComponent>(tutorialButton);

    if (GLFWFunctions::tutorialCurrentPage >= 1 && GLFWFunctions::tutorialCurrentPage < 8)
    {
        nextTransform.scale.SetX(100.f);
        nextTransform.scale.SetY(130.f);
    }

    else
    {
        nextTransform.scale.SetX(0.f);
        nextTransform.scale.SetY(0.f);
    }

    if (GLFWFunctions::tutorialCurrentPage > 1 && GLFWFunctions::tutorialCurrentPage <= 8)
    {
        previousTransform.scale.SetX(100.f);
        previousTransform.scale.SetY(130.f);
    }

    else
    {
        previousTransform.scale.SetX(0.f);
        previousTransform.scale.SetY(0.f);
    }
    
    if (GLFWFunctions::tutorialCurrentPage == 1 || GLFWFunctions::tutorialCurrentPage == 3 || GLFWFunctions::tutorialCurrentPage == 4 ||
        GLFWFunctions::tutorialCurrentPage == 6 || GLFWFunctions::tutorialCurrentPage == 8)
    {
		tutorialTransform.scale.SetX(0.f);
		tutorialTransform.scale.SetY(0.f);
	}
    else {
        tutorialTransform.scale.SetX(300.f);
        tutorialTransform.scale.SetY(150.f);
    }
}

// this is to update the level complete menu buttons in real-time
void GraphicSystemECS::updateButtons()
{
    Entity nextLevelButton = ecsCoordinator.getEntityFromID("nextLevelButton");
    Entity mainMenuButton = ecsCoordinator.getEntityFromID("mainMenuButton");

    TransformComponent& nextLevelTransform = ecsCoordinator.getComponent<TransformComponent>(nextLevelButton);
    TransformComponent& mainMenuTransform = ecsCoordinator.getComponent<TransformComponent>(mainMenuButton);

    if (GameViewWindow::getSceneNum() > 4)
    {
        nextLevelTransform.scale.SetX(0.f);
        nextLevelTransform.scale.SetY(0.f);
        mainMenuTransform.position.SetX(5.f);
    }

    else
    {
        nextLevelTransform.scale.SetX(260.f);
        nextLevelTransform.scale.SetY(130.f);
        mainMenuTransform.position.SetX(125.f);
    }
}



void GraphicSystemECS::cleanup() {}

std::string GraphicSystemECS::getSystemECS() {
    return "GraphicsSystemECS";
}