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
    //find out how many layers there are
    //for each layer draw entities in that layer
	//start from 0 to highest layer (so 0 is drawn first)
    // Compute view matrix
    // Get player entity and transform once at start of frame
    Entity playerEntity = Entity{};
    TransformComponent* playerTransform = nullptr;

    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
            playerEntity = entity;
            playerTransform = &ecsCoordinator.getComponent<TransformComponent>(entity);
            break;
        }

    }
    Entity fpsEntity = ecsCoordinator.getFPSDisplayEntity();
    if (fpsEntity != 0) {
       
        if (ecsCoordinator.hasComponent<FontComponent>(fpsEntity)) {
            auto& font = ecsCoordinator.getComponent<FontComponent>(fpsEntity);

           
            if (GLFWFunctions::showFPS) {
                char fpsText[32];
                snprintf(fpsText, sizeof(fpsText), "FPS:%.0f", GLFWFunctions::fps);
                font.text = fpsText;
            }
            else {
                font.text = ""; 
            }
        }
    }

    // Update camera once per frame
    if (GLFWFunctions::allow_camera_movement) {
        cameraSystem.update();
    }
    else if (playerTransform != nullptr) {
        cameraSystem.lockToComponent(*playerTransform);
        cameraSystem.update();
    }

    for (int i = 0; i < layerManager.getLayerCount(); i++) {
        //check if layer is visible
        bool isLayerVisible = layerManager.getLayerVisibility(i);
        if (isLayerVisible) {
            for (auto entity : layerManager.getEntitiesFromLayer(i)) {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                Console::GetLog() << "Entity: " << entity << " Position: " << transform.position.GetX() << ", " << transform.position.GetY() << std::endl;

                bool hasAnimation = ecsCoordinator.hasComponent<AnimationComponent>(entity);
                AnimationComponent animation{};


                // Check if the entity has an animation component
                if (hasAnimation) {
                    animation = ecsCoordinator.getComponent<AnimationComponent>(entity);

                    if (GameViewWindow::getPaused() || GLFWFunctions::gamePaused) {
                        // Only call Update() to calculate UVs without advancing the frame
                        animation.isAnimated = false;
                        animation.UpdateUVCoordinates();
                    }
                    else {
                        animation.isAnimated = true;
                        animation.Update();
                    }
                }

                /*AnimationData animationData(animation.totalFrames, animation.frameTime, animation.columns, animation.rows);

                if (GameViewWindow::getPaused()) {
                    animationData.SetCurrentFrame(0);
                    animation.isAnimated = false;
                }*/

                auto entitySig = ecsCoordinator.getEntitySignature(entity);

                bool isPlayer = ecsCoordinator.hasComponent<PlayerComponent>(entity);
                bool isButton = ecsCoordinator.hasComponent<ButtonComponent>(entity);
                bool isUI = ecsCoordinator.hasComponent<UIComponent>(entity);
                bool isFilter = ecsCoordinator.hasComponent<FilterComponent>(entity);
                //bool isExit = ecsCoordinator.hasComponent<ExitComponent>(entity);

				bool isEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);

				auto& behaviour = ecsCoordinator.getComponent<BehaviourComponent>(entity);

                // Use hasMovement for the update parameter
                //graphicsSystem.Update(dt / 10.0f, isAnimate || hasMovement || isEnemy);


                //graphicsSystem.Update(dt / 10.0f, (isAnimate&& isPump) || (isPlayer && hasMovement) || (isEnemy && hasMovement)); // Use hasMovement instead of true
                myMath::Matrix3x3 identityMatrix = { 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f };
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, cameraSystem.getViewMatrix());


                mouseBehaviour.update(entity);

                bool isNavigation = ecsCoordinator.hasComponent<NavigationComponent>(entity);

                if (isNavigation) {
                    auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(entity);

                    // Only render navigation arrows if they're set to visible
                    if (!navComp.isVisible) {
                        continue;
                    }

                    // Use identity matrix for navigation UI to keep it on screen regardless of camera
                    if (ecsCoordinator.getEntityID(entity) == "nav_arrow") {
                        // The model transformation needs to account for the camera position but not rotation
                        // For UI elements like arrows that need to follow the camera but stay at screen edges
                        transform.mdl_xform = graphicsSystem.UpdateObject(
                            transform.position,
                            transform.scale,
                            transform.orientation,
                            cameraSystem.getViewMatrix());
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
                    if (isButton || isUI)
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

                if (ecsCoordinator.getEntityID(entity) == "cutsceneBg")
                {
                    ecsCoordinator.setTextureID(entity, "cutsceneBackground");
                }
                if (ecsCoordinator.getEntityID(entity) == "gameLogo")
                {
                    ecsCoordinator.setTextureID(entity, "gameLogo");
                }

                if (ecsCoordinator.getEntityID(entity) == "pauseMenuBg")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                    ecsCoordinator.setTextureID(entity, "pauseMenu");
                }

                if (ecsCoordinator.getEntityID(entity) == "optionsMenuBg")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                    ecsCoordinator.setTextureID(entity, "optionsMenu");
                }

                if (ecsCoordinator.getEntityID(entity) == "tutorialBaseBg")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                    const std::vector<std::string> textureIds = { "tutorialControlsBase", "tutorialMovementBase", 
                                                                  "tutorialKeyItems1Base", "tutorialKeyItems2Base", 
                                                                  "tutorialWaterCurrentBase", "tutorialEscapeBase", 
                                                                  "tutorialFilterBase", "tutorialFishBase" };

                    int currentPage = GLFWFunctions::tutorialCurrentPage;
                    if (currentPage >= 1 && currentPage <= textureIds.size())
                    {
                        ecsCoordinator.setTextureID(entity, textureIds[currentPage - 1]);
                    }
                }

                if (ecsCoordinator.getEntityID(entity) == "pageCounter")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                    const std::vector<std::string> textureIds = { "pageCounter1", "pageCounter2", "pageCounter3",
                                                                  "pageCounter4", "pageCounter5", "pageCounter6",
                                                                  "pageCounter7", "pageCounter8"};

                    int currentPage = GLFWFunctions::tutorialCurrentPage;
                    if (currentPage >= 1 && currentPage <= textureIds.size())
                    {
                        ecsCoordinator.setTextureID(entity, textureIds[currentPage - 1]);
                    }
                }

                if (ecsCoordinator.getEntityID(entity) == "rotationSpeedSliderNotch")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                    ecsCoordinator.setTextureID(entity, "rotationSpeedNotch");
                }

                if (ecsCoordinator.getEntityID(entity) == "quitLevelMenuBase")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                    ecsCoordinator.setTextureID(entity, "quitLevelBase");
                }

                if (ecsCoordinator.getEntityID(entity) == "levelCompletedMenuBase")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                    ecsCoordinator.setTextureID(entity, "levelCompletedBase");
                }

                if (ecsCoordinator.getEntityID(entity) == "gameOverBG")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                    ecsCoordinator.setTextureID(entity, "GameOverBG");
                }

        if (ecsCoordinator.getEntityID(entity).find("sfxNotch") != std::string::npos ||
            ecsCoordinator.getEntityID(entity).find("musicNotch") != std::string::npos)
        {
            // initialize the values that counts the number active notches for sfx and music
            // audio, start and end x position for both sfx and music audio
            int activeNotchesSFX{}, activeNotchesMusic{};
            float startPosSFX{}, endPosSFX{};
            float startPosMusic{}, endPosMusic{};

                    TransformComponent sfxArrowTransform{}, musicArrowTransform{};

            // Identify arrow positions for SFX and Music
            for (auto& arrowEntity : ecsCoordinator.getAllLiveEntities())
            {
                // below checks which audio arrow entity Id it is and it will get the transform
                // component of that particular audio arrow entity
                std::string arrowId = ecsCoordinator.getEntityID(arrowEntity);
                if (arrowId == "sfxSoundbarArrow")
                {
                    sfxArrowTransform = ecsCoordinator.getComponent<TransformComponent>(arrowEntity);
                }
                
                else if (arrowId == "musicSoundbarArrow")
                {
                    musicArrowTransform = ecsCoordinator.getComponent<TransformComponent>(arrowEntity);
                }
            }

            // a vector that stores pairs of entity and their corresponding components
            // for sfx and music notches 
            std::vector<std::pair<Entity, TransformComponent>> sfxNotches, musicNotches;

            // Separate sfx and music notches
            for (auto& notchEntity : ecsCoordinator.getAllLiveEntities())
            {
                std::string notchId = ecsCoordinator.getEntityID(notchEntity);
                TransformComponent notchTransform = ecsCoordinator.getComponent<TransformComponent>(notchEntity);

                // add a sfx notch to the sfxNotches vector
                if (notchId.find("sfxNotch") != std::string::npos)
                {
                    sfxNotches.emplace_back(notchEntity, notchTransform);
                }

                // add a music notch to the musicNotches vector
                else if (notchId.find("musicNotch") != std::string::npos)
                {
                    musicNotches.emplace_back(notchEntity, notchTransform);
                }
            }

                    float sfxPercentage{}, musicPercentage{};

            // ensuring that there are 10 elements inside the sfxNotches vector
            if (sfxNotches.size() == 10)
            {
                // calculating the start x position of the first sfx notch
                startPosSFX = sfxNotches[0].second.position.GetX() - (sfxNotches[0].second.scale.GetX() / 2.0f);
                // calculating the end x position of the tenth sfx notch
                endPosSFX = sfxNotches[9].second.position.GetX() + (sfxNotches[9].second.scale.GetX() / 2.0f);
                // below are the codes that calculates the sfx audio percentage
                float arrowPosSFX = sfxArrowTransform.position.GetX();
                float progressSFX = std::abs((arrowPosSFX - startPosSFX) / (endPosSFX - startPosSFX));
                sfxPercentage = std::round(progressSFX * 10.f) * 10.f;
                sfxPercentage = std::clamp(sfxPercentage, 0.f, 100.f);
                AudioSystem::sfxPercentage = sfxPercentage;
            }

            // ensuring that there are 10 elements inside the musicNotches vector
            if (musicNotches.size() == 10)
            {
                // calculating the start x position of the first music notch
                startPosMusic = musicNotches[0].second.position.GetX() - (musicNotches[0].second.scale.GetX() / 2.0f);
                // calculating the end x position of the tenth music notch
                endPosMusic = musicNotches[9].second.position.GetX() + (musicNotches[9].second.scale.GetX() / 2.0f);
                // below are the codes that calculates the music audio percentage
                float arrowPosMusic = musicArrowTransform.position.GetX();
                float progressMusic = std::abs((arrowPosMusic - startPosMusic) / (endPosMusic - startPosMusic));
                musicPercentage = std::round(progressMusic * 10.f) * 10.f;
                musicPercentage = std::clamp(musicPercentage, 0.f, 100.f);
                AudioSystem::musicPercentage = musicPercentage;
            }

                    // Update active notches separately for SFX and Music
                    for (size_t j = 0; j < sfxNotches.size(); ++j)
                    {
                        if (sfxArrowTransform.position.GetX() + sfxArrowTransform.scale.GetX() / 2.35f >= sfxNotches[j].second.position.GetX())
                        {
                            activeNotchesSFX = static_cast<int>(j) + 1;
                        }
                    }

                    for (size_t j = 0; j < musicNotches.size(); ++j)
                    {
                        if (musicArrowTransform.position.GetX() + musicArrowTransform.scale.GetX() / 2.35f >= musicNotches[j].second.position.GetX())
                        {
                            activeNotchesMusic = static_cast<int>(j) + 1;
                        }
                    }

                    // Draw SFX Notches
                    for (size_t j = 0; j < sfxNotches.size(); ++j)
                    {
                        std::string notchTexture = (j < activeNotchesSFX) ? "activeSoundbarNotch" : "unactiveSoundbarNotch";
                        TransformComponent& notchTransform = sfxNotches[j].second;
                        notchTransform.mdl_xform = graphicsSystem.UpdateObject(notchTransform.position, notchTransform.scale, notchTransform.orientation, identityMatrix);
                        graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(notchTexture), notchTransform.mdl_xform, animation.currentUVs);
                    }

                    // Draw Music Notches
                    for (size_t j = 0; j < musicNotches.size(); ++j)
                    {
                        std::string notchTexture = (j < activeNotchesMusic) ? "activeSoundbarNotch" : "unactiveSoundbarNotch";
                        TransformComponent& notchTransform = musicNotches[j].second;
                        notchTransform.mdl_xform = graphicsSystem.UpdateObject(notchTransform.position, notchTransform.scale, notchTransform.orientation, identityMatrix);
                        graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(notchTexture), notchTransform.mdl_xform, animation.currentUVs);
                    }
                }

                if (ecsCoordinator.getEntityID(entity) == "sfxSoundbarArrow" || ecsCoordinator.getEntityID(entity) == "musicSoundbarArrow")
                {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                    ecsCoordinator.setTextureID(entity, "soundbarArrow");
                    //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("soundbarArrow"), transform.mdl_xform);
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

                if (behaviour.exit) {
                    //transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                    if (GLFWFunctions::collectableCount == 0) {
                        ecsCoordinator.setTextureID(entity, "cloggedVent3");
                    }
                    else if (GLFWFunctions::collectableCount == 1) {
                        ecsCoordinator.setTextureID(entity, "cloggedVent2");
                    }
                    else if (GLFWFunctions::collectableCount == 2) {
                        ecsCoordinator.setTextureID(entity, "cloggedVent1");
                    }
                    else if (GLFWFunctions::collectableCount >= 3) {
                        ecsCoordinator.setTextureID(entity, "cloggedVent0");
                    }
                }

                if (isButton) {
                    transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                    if (ecsCoordinator.getEntityID(entity) == "quitButton") {
                        ecsCoordinator.setTextureID(entity, "buttonQuit");
                        //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("buttonQuit"), transform.mdl_xform);
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "retryButton")
                    {
                        ecsCoordinator.setTextureID(entity, "buttonRetry");
                        //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("buttonRetry"), transform.mdl_xform);
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "pauseRetryButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveRetryButton");
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeRetryButton");
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "startButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveStartButton");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveStartButton"), transform.mdl_xform);
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeStartButton");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("activeStartButton"), transform.mdl_xform);
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "resumeButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveResumeButton");
                            // graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveResumeButton"), transform.mdl_xform);
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeResumeButton");
                            // graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("activeResumeButton"), transform.mdl_xform);
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "optionsButton" || ecsCoordinator.getEntityID(entity) == "pauseOptionsButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveOptionsButton");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveOptionsButton"), transform.mdl_xform);
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeOptionsButton");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("activeOptionsButton"), transform.mdl_xform);
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "tutorialButton" || ecsCoordinator.getEntityID(entity) == "pauseTutorialButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveTutorialButton");
                            // graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveTutorialButton"), transform.mdl_xform);
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeTutorialButton");
                            // graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("activeTutorialButton"), transform.mdl_xform);
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "confirmButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveConfirmButton");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveConfirmButton"), transform.mdl_xform);
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeConfirmButton");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("activeConfirmButton"), transform.mdl_xform);
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "quitWindowButton" || ecsCoordinator.getEntityID(entity) == "pauseQuitButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveQuitButton");
                            // graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveQuitButton"), transform.mdl_xform);
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeQuitButton");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("activeQuitButton"), transform.mdl_xform);
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "quitToMainMenuButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveYesButton");
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeYesButton");
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "returnToPauseMenuButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveNoButton");
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeNoButton");
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "nextLevelButton")
                    {
                        updateButtons();

                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveNextLevelButton");
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeNextLevelButton");
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "mainMenuButton")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "unactiveMainMenuButton");
                        }

                        else
                        {
                            ecsCoordinator.setTextureID(entity, "activeMainMenuButton");
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "tutorialClick")
                    {
                        if (ecsCoordinator.getEntityID(entity) != mouseBehaviour.getHoveredButton())
                        {
                            ecsCoordinator.setTextureID(entity, "tutorial_inactive");
                            
                        }
                        else
                        {
                            ecsCoordinator.setTextureID(entity, "tutorial_active");
                      
                        }
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "closePauseMenu" || ecsCoordinator.getEntityID(entity) == "closeOptionsMenu" || 
                             ecsCoordinator.getEntityID(entity) == "closeTutorialMenu")
                    {
                        ecsCoordinator.setTextureID(entity, "closePopupButton");
                        // graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("closePopupButton"), transform.mdl_xform);
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "sfxSoundbarBase" || ecsCoordinator.getEntityID(entity) == "musicSoundbarBase")
                    {
                        std::string soundbarType = ecsCoordinator.getEntityID(entity);
                        TransformComponent soundbarTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
                        TransformComponent arrowTransform{};

                        std::string audioArrowId = (soundbarType == "sfxSoundbarBase") ? "sfxSoundbarArrow" :
                                                   (soundbarType == "musicSoundbarBase") ? "musicSoundbarArrow" : "";

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
                            ecsCoordinator.setTextureID(entity, "soundbarBase");
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("soundbarBase"), transform.mdl_xform);

                            //float soundbarLeftBoundary = soundbarTransform.position.GetX() - (soundbarTransform.scale.GetX() / 2.2f);

                            //std::string textureName = (arrowTransform.position.GetX() <= soundbarLeftBoundary) ? "activeSoundbar" : "unactiveSoundbar";
                            //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(textureName), transform.mdl_xform);
                        }


                        //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("unactiveSoundbar"), transform.mdl_xform);
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "nextTutorialPage")
                    {
                        ecsCoordinator.setTextureID(entity, "rightArrow");
                        updateTutorialArrows();
                    }


                    else if (ecsCoordinator.getEntityID(entity) == "previousTutorialPage")
                    {
                        ecsCoordinator.setTextureID(entity, "leftArrow");
                        updateTutorialArrows();
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "rotationSpeedSlider")
                    {
                        TransformComponent sliderTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
                        TransformComponent notchTransform{};

                        
                        for (auto& sliderNotchEntity : ecsCoordinator.getAllLiveEntities())
                        {
                            if (ecsCoordinator.getEntityID(sliderNotchEntity) == "rotationSpeedSliderNotch")
                            {
                                notchTransform = ecsCoordinator.getComponent<TransformComponent>(sliderNotchEntity);
                                break;
                            }
                        }

                        ecsCoordinator.setTextureID(entity, "rotationSpeedSlider");
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "gameOverRetryButton")
                    {
                        ecsCoordinator.setTextureID(entity, "buttonRetry");
                    }

                    else if (ecsCoordinator.getEntityID(entity) == "gameOverQuitButton")
                    {
                        ecsCoordinator.setTextureID(entity, "buttonQuit");
                    }
                }

                //if is player, check visibility of player, if not visible do not render
                if (isPlayer) {
                    auto& player = ecsCoordinator.getComponent<PlayerComponent>(entity);

                    // Skip invisible players
                    if (!player.isVisible) {
                        continue;
                    }

                    
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
                    auto& velocity = physics.velocity;
                    float velocityMagnitude = std::sqrt(velocity.GetX() * velocity.GetX() + velocity.GetY() * velocity.GetY());

                    
                    handlePlayerMovementAnimation(entity, transform, animation, velocityMagnitude);

                    // Skip further rendering for this entity
                    continue;
                    
                }

                if (isFilter) {
                    auto& filter = ecsCoordinator.getComponent<FilterComponent>(entity);
                    if (filter.isFilterClogged) {
                        ecsCoordinator.setTextureID(entity, "filter_mossed");
                    }
                    else {
                        ecsCoordinator.setTextureID(entity, "exitFilter");
                    }
                }

                if (isEnemy)
                {
					auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);
      //              if (enemy.drawVisionDebug) {
						//auto& transformation = ecsCoordinator.getComponent<TransformComponent>(entity);
						//graphicsSystem.drawDebugVisionCone(transformation, enemy.visionAngle, enemy.visionDistance, cameraSystem.getViewMatrix());
      //                  graphicsSystem.drawDebugVisionCone(transformation, enemy.visionAngle, (enemy.visionDistance / 3.0f), cameraSystem.getViewMatrix());
      //              }

                    if (enemy.currState == 0) {
                        ecsCoordinator.setTextureID(entity, "goldfish");
                        auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                        enemyAnimation.totalFrames = 24;
                        enemyAnimation.columns = 4;
                        enemyAnimation.rows = 6;
                    }
                    //else if (enemy.currState == 1) {
                    //    ecsCoordinator.setTextureID(entity, "goldfishAlert");
                    //    auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                    //    enemyAnimation.totalFrames = 5;
                    //    enemyAnimation.columns = 2;
                    //    enemyAnimation.rows = 3;
                    //}
                    else if (enemy.currState == 2) {
                        ecsCoordinator.setTextureID(entity, "goldfishBite");
						auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                        enemyAnimation.totalFrames = 16;
                        enemyAnimation.columns = 4;
                        enemyAnimation.rows = 4;
                    }

                    if (ecsCoordinator.getTextureID(entity) == "goldfishAlert")
                    {
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

                if (ecsCoordinator.getEntityID(entity) == "collectAnimation" || ecsCoordinator.getEntityID(entity) == "filterPush") {
                    auto& anim = ecsCoordinator.getComponent<AnimationComponent>(entity);

                    double currentAbsoluteTime = glfwGetTime();
                    double timeSinceCreation = currentAbsoluteTime - anim.creationTime;

                    anim.currentFrame = static_cast<int>((timeSinceCreation / anim.frameTime)) % static_cast<int>(anim.totalFrames);

                    if (anim.currentFrame == static_cast<int>(anim.totalFrames) - 1) {
                        ecsCoordinator.destroyEntity(entity); 
                    }
                }

                if (ecsCoordinator.getEntityID(entity) == "fishAlertAnimation") {
                    auto& anim = ecsCoordinator.getComponent<AnimationComponent>(entity);

                    double currentAbsoluteTime = glfwGetTime();
                    double timeSinceCreation = currentAbsoluteTime - anim.creationTime;

                    anim.currentFrame = static_cast<int>((timeSinceCreation / anim.frameTime)) % static_cast<int>(anim.totalFrames);

                    if (anim.currentFrame == static_cast<int>(anim.totalFrames) - 1) {

                        ecsCoordinator.destroyEntity(entity);
                    }
                }

                if (ecsCoordinator.getEntityID(entity) == "fishAttackAnimation") {
                    auto& anim = ecsCoordinator.getComponent<AnimationComponent>(entity);

                    double currentAbsoluteTime = glfwGetTime();
                    double timeSinceCreation = currentAbsoluteTime - anim.creationTime;

                    anim.currentFrame = static_cast<int>((timeSinceCreation / anim.frameTime)) % static_cast<int>(anim.totalFrames);

                    if (anim.currentFrame == static_cast<int>(anim.totalFrames) - 1) {

                        ecsCoordinator.destroyEntity(entity);
                    }
                }

                

                if (ecsCoordinator.getTextureID(entity) != "") {
                    //render filter in animation when filter is not clogged
                    if (ecsCoordinator.getTextureID(entity) == "filter_in.png") {
                        if (GLFWFunctions::filterClogged) {
                            continue;
                        }
                    }
					//if (ecsCoordinator.getTextureID(entity) == "filter-out.png") {
					//	if (GLFWFunctions::filterClogged) {
					//		continue;
					//	}
					//}
                    //should not render the bubble animation if pump is not on
                    if (ecsCoordinator.getTextureID(entity) == "bubbles 3.png") {
                        if (!GLFWFunctions::isPumpOn) {
                            continue;
                        }
                    }
                    graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(ecsCoordinator.getTextureID(entity)), transform.mdl_xform, animation.currentUVs);
                }

                //std::cout << "isPump on? " << (GLFWFunctions::isPumpOn ? "true" : "false") << std::endl;

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

    if (GLFWFunctions::tutorialCurrentPage == 1 || GLFWFunctions::tutorialCurrentPage == 6 || GLFWFunctions::tutorialCurrentPage == 8) 
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
        mainMenuTransform.position.SetX(-5.f);
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