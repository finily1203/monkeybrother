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
#include <vector>
#include <unordered_map>

// Cache frequently accessed entity types to avoid string comparisons
enum class EntityType {
    Unknown,
    Player,
    Button,
    CollectableUI,
    PauseMenuBg,
    OptionsMenuBg,
    TutorialBaseBg,
    PageCounter,
    SliderNotch,
    QuitLevelMenu,
    LevelCompletedMenu,
    GameOverBg,
    SoundbarArrow,
    SfxNotch,
    MusicNotch,
    SoundbarBase,
    TutorialPageNav,
    RotationSlider,
    Exit,
    Filter,
    CutsceneBg,
    GameLogo
};

// Cache to store entity IDs to types mapping
std::unordered_map<std::string, EntityType> entityTypeMap;

// Initialize entity type mapping in constructor
GraphicSystemECS::GraphicSystemECS() {
    // Pre-populate the entity type map for common entities
    entityTypeMap = {
        {"player", EntityType::Player},
        {"pauseMenuBg", EntityType::PauseMenuBg},
        {"optionsMenuBg", EntityType::OptionsMenuBg},
        {"tutorialBaseBg", EntityType::TutorialBaseBg},
        {"pageCounter", EntityType::PageCounter},
        {"rotationSpeedSliderNotch", EntityType::SliderNotch},
        {"quitLevelMenuBase", EntityType::QuitLevelMenu},
        {"levelCompletedMenuBase", EntityType::LevelCompletedMenu},
        {"gameOverBG", EntityType::GameOverBg},
        {"sfxSoundbarArrow", EntityType::SoundbarArrow},
        {"musicSoundbarArrow", EntityType::SoundbarArrow},
        {"sfxSoundbarBase", EntityType::SoundbarBase},
        {"musicSoundbarBase", EntityType::SoundbarBase},
        {"nextTutorialPage", EntityType::TutorialPageNav},
        {"previousTutorialPage", EntityType::TutorialPageNav},
        {"rotationSpeedSlider", EntityType::RotationSlider},
        {"exit", EntityType::Exit},
        {"cutsceneBg", EntityType::CutsceneBg},
        {"gameLogo", EntityType::GameLogo},
        {"collectableUI", EntityType::CollectableUI}
    };
}

// Helper function to get entity type
EntityType GraphicSystemECS::getEntityType(const std::string& entityId) {
    auto it = entityTypeMap.find(entityId);
    if (it != entityTypeMap.end()) {
        return it->second;
    }

    // Handle prefixes for notches
    if (entityId.find("sfxNotch") != std::string::npos) {
        return EntityType::SfxNotch;
    }
    else if (entityId.find("musicNotch") != std::string::npos) {
        return EntityType::MusicNotch;
    }

    return EntityType::Unknown;
}

// Cache for UI entities
void GraphicSystemECS::cacheUIEntities() {
    sfxNotchEntities.clear();
    musicNotchEntities.clear();
    sfxArrowEntity = 0;
    musicArrowEntity = 0;

    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        std::string id = ecsCoordinator.getEntityID(entity);
        if (id.find("sfxNotch") != std::string::npos) {
            sfxNotchEntities.push_back(entity);
        }
        else if (id.find("musicNotch") != std::string::npos) {
            musicNotchEntities.push_back(entity);
        }
        else if (id == "sfxSoundbarArrow") {
            sfxArrowEntity = entity;
        }
        else if (id == "musicSoundbarArrow") {
            musicArrowEntity = entity;
        }
    }

    uiCacheInitialized = true;
}

//Initialise currently does not do anything
void GraphicSystemECS::initialise() {
    // Initialize static matrices
    identityMatrix = myMath::Matrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

double GraphicSystemECS::elapsedTimeSinceGrowStart(const PlayerComponent& player) {
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

// Handle sound bar notches rendering
void GraphicSystemECS::handleSoundBarNotches() {
    if (!uiCacheInitialized) {
        cacheUIEntities();
    }

    // Get transform components for arrows
    if (sfxArrowEntity != 0 && musicArrowEntity != 0) {
        TransformComponent sfxArrowTransform = ecsCoordinator.getComponent<TransformComponent>(sfxArrowEntity);
        TransformComponent musicArrowTransform = ecsCoordinator.getComponent<TransformComponent>(musicArrowEntity);

        float sfxPercentage = 0.0f, musicPercentage = 0.0f;
        int activeNotchesSFX = 0, activeNotchesMusic = 0;

        // Handle SFX notches
        if (!sfxNotchEntities.empty()) {
            std::sort(sfxNotchEntities.begin(), sfxNotchEntities.end(), [this](Entity a, Entity b) {
                auto& transformA = ecsCoordinator.getComponent<TransformComponent>(a);
                auto& transformB = ecsCoordinator.getComponent<TransformComponent>(b);
                return transformA.position.GetX() < transformB.position.GetX();
                });

            // Calculate start and end positions
            auto& firstNotch = ecsCoordinator.getComponent<TransformComponent>(sfxNotchEntities.front());
            auto& lastNotch = ecsCoordinator.getComponent<TransformComponent>(sfxNotchEntities.back());
            float startPosSFX = firstNotch.position.GetX() - (firstNotch.scale.GetX() / 2.0f);
            float endPosSFX = lastNotch.position.GetX() + (lastNotch.scale.GetX() / 2.0f);

            // Calculate percentage
            float arrowPosSFX = sfxArrowTransform.position.GetX();
            float progressSFX = std::abs((arrowPosSFX - startPosSFX) / (endPosSFX - startPosSFX));
            sfxPercentage = std::round(progressSFX * 10.f) * 10.f;
            sfxPercentage = std::clamp(sfxPercentage, 0.f, 100.f);
            AudioSystem::sfxPercentage = sfxPercentage;

            // Determine active notches
            for (size_t j = 0; j < sfxNotchEntities.size(); ++j) {
                auto& notchTransform = ecsCoordinator.getComponent<TransformComponent>(sfxNotchEntities[j]);
                if (sfxArrowTransform.position.GetX() + sfxArrowTransform.scale.GetX() / 2.35f >= notchTransform.position.GetX()) {
                    activeNotchesSFX = static_cast<int>(j) + 1;
                }
            }

            // Draw SFX notches
            for (size_t j = 0; j < sfxNotchEntities.size(); ++j) {
                std::string notchTexture = (j < activeNotchesSFX) ? "activeSoundbarNotch" : "unactiveSoundbarNotch";
                Entity notchEntity = sfxNotchEntities[j];
                auto& notchTransform = ecsCoordinator.getComponent<TransformComponent>(notchEntity);
                notchTransform.mdl_xform = graphicsSystem.UpdateObject(notchTransform.position, notchTransform.scale, notchTransform.orientation, identityMatrix);
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(notchTexture), notchTransform.mdl_xform, cachedEmptyUVs);
            }
        }

        // Handle Music notches (similar logic as SFX)
        if (!musicNotchEntities.empty()) {
            std::sort(musicNotchEntities.begin(), musicNotchEntities.end(), [this](Entity a, Entity b) {
                auto& transformA = ecsCoordinator.getComponent<TransformComponent>(a);
                auto& transformB = ecsCoordinator.getComponent<TransformComponent>(b);
                return transformA.position.GetX() < transformB.position.GetX();
                });

            auto& firstNotch = ecsCoordinator.getComponent<TransformComponent>(musicNotchEntities.front());
            auto& lastNotch = ecsCoordinator.getComponent<TransformComponent>(musicNotchEntities.back());
            float startPosMusic = firstNotch.position.GetX() - (firstNotch.scale.GetX() / 2.0f);
            float endPosMusic = lastNotch.position.GetX() + (lastNotch.scale.GetX() / 2.0f);

            float arrowPosMusic = musicArrowTransform.position.GetX();
            float progressMusic = std::abs((arrowPosMusic - startPosMusic) / (endPosMusic - startPosMusic));
            musicPercentage = std::round(progressMusic * 10.f) * 10.f;
            musicPercentage = std::clamp(musicPercentage, 0.f, 100.f);
            AudioSystem::musicPercentage = musicPercentage;

            for (size_t j = 0; j < musicNotchEntities.size(); ++j) {
                auto& notchTransform = ecsCoordinator.getComponent<TransformComponent>(musicNotchEntities[j]);
                if (musicArrowTransform.position.GetX() + musicArrowTransform.scale.GetX() / 2.35f >= notchTransform.position.GetX()) {
                    activeNotchesMusic = static_cast<int>(j) + 1;
                }
            }

            for (size_t j = 0; j < musicNotchEntities.size(); ++j) {
                std::string notchTexture = (j < activeNotchesMusic) ? "activeSoundbarNotch" : "unactiveSoundbarNotch";
                Entity notchEntity = musicNotchEntities[j];
                auto& notchTransform = ecsCoordinator.getComponent<TransformComponent>(notchEntity);
                notchTransform.mdl_xform = graphicsSystem.UpdateObject(notchTransform.position, notchTransform.scale, notchTransform.orientation, identityMatrix);
                graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(notchTexture), notchTransform.mdl_xform, cachedEmptyUVs);
            }
        }
    }
}

// Batch rendering function
void GraphicSystemECS::batchRender(int layerIndex) {
    // Use a map to batch entities by texture
    std::unordered_map<std::string, std::vector<Entity>> renderBatches;

    // Group entities by texture ID
    for (auto entity : layerManager.getEntitiesFromLayer(layerIndex)) {
        std::string textureId = ecsCoordinator.getTextureID(entity);
        if (!textureId.empty()) {
            renderBatches[textureId].push_back(entity);
        }
    }

    // Render each batch
    for (const auto& [textureId, entities] : renderBatches) {
        GLuint texture = assetsManager.GetTexture(textureId);

        for (Entity entity : entities) {
            auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

            // Update transform
            if (ecsCoordinator.hasComponent<UIComponent>(entity) ||
                ecsCoordinator.hasComponent<ButtonComponent>(entity)) {
                transform.mdl_xform = graphicsSystem.UpdateObject(
                    transform.position, transform.scale, transform.orientation, identityMatrix);
            }
            else {
                transform.mdl_xform = graphicsSystem.UpdateObject(
                    transform.position, transform.scale, transform.orientation,
                    cameraSystem.getViewMatrix());
            }

            // Get animation UVs if available, otherwise use defaults
            std::vector<glm::vec2> uvs = cachedEmptyUVs;
            if (ecsCoordinator.hasComponent<AnimationComponent>(entity)) {
                auto& animation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                uvs = animation.currentUVs;
            }

            // Draw the entity
            graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, texture,
                transform.mdl_xform, uvs);
        }
    }
}

//Update function to update the graphics system
//uses functions from GraphicsSystem class to update, draw
//and render objects.
void GraphicSystemECS::update(float dt) {
    (void)dt;

    // Initialize empty UVs if needed
    if (cachedEmptyUVs.empty()) {
        cachedEmptyUVs.resize(4);
        cachedEmptyUVs[0] = glm::vec2(1.0f, 1.0f);
        cachedEmptyUVs[1] = glm::vec2(1.0f, 0.0f);
        cachedEmptyUVs[2] = glm::vec2(0.0f, 0.0f);
        cachedEmptyUVs[3] = glm::vec2(0.0f, 1.0f);
    }

    // Reset UI cache on scene change
    if (GLFWFunctions::newSceneLoaded) {
        uiCacheInitialized = false;
        GLFWFunctions::newSceneLoaded = false;
    }

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

    // Update FPS display
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

    // Cache common matrices
    viewMatrix = cameraSystem.getViewMatrix();

    // Process all animation components once
    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<AnimationComponent>(entity)) {
            auto& animation = ecsCoordinator.getComponent<AnimationComponent>(entity);

            if (GameViewWindow::getPaused() || GLFWFunctions::gamePaused) {
                // Only calculate UVs without advancing the frame
                animation.isAnimated = false;
                animation.UpdateUVCoordinates();
            }
            else {
                animation.isAnimated = true;
                animation.Update();
            }
        }
    }

    // Special handling for sound bar notches
    handleSoundBarNotches();

    // Render each layer
    for (int i = 0; i < layerManager.getLayerCount(); i++) {
        // Check if layer is visible
        bool isLayerVisible = layerManager.getLayerVisibility(i);
        if (!isLayerVisible) continue;

        // Get all entities in this layer
        const auto& layerEntities = layerManager.getEntitiesFromLayer(i);

        // Process special entities like Player separately
        for (auto entity : layerEntities) {
            auto entityId = ecsCoordinator.getEntityID(entity);

            // Update entity flags and components
            bool isPlayer = ecsCoordinator.hasComponent<PlayerComponent>(entity);
            bool hasAnimation = ecsCoordinator.hasComponent<AnimationComponent>(entity);

            if (isPlayer && hasAnimation) {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                auto& animation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                auto& player = ecsCoordinator.getComponent<PlayerComponent>(entity);

                // Skip invisible players
                if (!player.isVisible) continue;

                // Handle player animations
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
                continue;
            }

            // Special handling for certain entity types
            EntityType type = getEntityType(entityId);

            switch (type) {
            case EntityType::CutsceneBg:
                ecsCoordinator.setTextureID(entity, "cutsceneBackground");
                break;

            case EntityType::GameLogo:
                ecsCoordinator.setTextureID(entity, "gameLogo");
                break;

            case EntityType::PauseMenuBg:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "pauseMenu");
                break;
            }

            case EntityType::OptionsMenuBg:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "optionsMenu");
                break;
            }

            case EntityType::TutorialBaseBg:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                const std::vector<std::string> textureIds = { "tutorialControlsBase", "tutorialMovementBase",
                                                              "tutorialKeyItems1Base", "tutorialKeyItems2Base",
                                                             "tutorialWaterCurrentBase", "tutorialEscapeBase",
                                                             "tutorialFilterBase", "tutorialFishBase" };

                int currentPage = GLFWFunctions::tutorialCurrentPage;
                if (currentPage >= 1 && currentPage <= static_cast<int>(textureIds.size()))
                {
                    ecsCoordinator.setTextureID(entity, textureIds[currentPage - 1]);
                }
                break;
            }

            case EntityType::PageCounter:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                const std::vector<std::string> textureIds = { "pageCounter1", "pageCounter2", "pageCounter3",
                                                             "pageCounter4", "pageCounter5", "pageCounter6",
                                                             "pageCounter7", "pageCounter8" };

                int currentPage = GLFWFunctions::tutorialCurrentPage;
                if (currentPage >= 1 && currentPage <= static_cast<int>(textureIds.size()))
                {
                    ecsCoordinator.setTextureID(entity, textureIds[currentPage - 1]);
                }
                break;
            }

            case EntityType::SliderNotch:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "rotationSpeedNotch");
                break;
            }

            case EntityType::QuitLevelMenu:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "quitLevelBase");
                break;
            }

            case EntityType::LevelCompletedMenu:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "levelCompletedBase");
                break;
            }

            case EntityType::GameOverBg:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "GameOverBG");
                break;
            }

            case EntityType::SoundbarArrow:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "soundbarArrow");
                break;
            }

            case EntityType::SoundbarBase:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, "soundbarBase");
                break;
            }

            case EntityType::TutorialPageNav:
            {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                if (entityId == "nextTutorialPage") {
                    ecsCoordinator.setTextureID(entity, "rightArrow");
                }
                else {
                    ecsCoordinator.setTextureID(entity, "leftArrow");
                }
                break;
            }

            default:
                break;
            }

            // Handle UI entities
            if (ecsCoordinator.hasComponent<UIComponent>(entity)) {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
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

            // Handle exit entities
            if (ecsCoordinator.hasComponent<BehaviourComponent>(entity)) {
                auto& behaviour = ecsCoordinator.getComponent<BehaviourComponent>(entity);
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
                    else if (GLFWFunctions::collectableCount >= 3) {
                        ecsCoordinator.setTextureID(entity, "cloggedVent0");
                    }
                }
            }

            // Handle button entities
            if (ecsCoordinator.hasComponent<ButtonComponent>(entity)) {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);

                // Update mouse interaction
                mouseBehaviour.update(entity);

                // Set the appropriate texture based on ID and hover state
                std::string hoveredButton = mouseBehaviour.getHoveredButton();

                // This is a button mapping that could be moved to a data structure
                if (entityId == "quitButton") {
                    ecsCoordinator.setTextureID(entity, "buttonQuit");
                }
                else if (entityId == "retryButton" || entityId == "gameOverRetryButton") {
                    ecsCoordinator.setTextureID(entity, "buttonRetry");
                }
                else if (entityId == "pauseRetryButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeRetryButton" : "unactiveRetryButton");
                }
                else if (entityId == "startButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeStartButton" : "unactiveStartButton");
                }
                else if (entityId == "resumeButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeResumeButton" : "unactiveResumeButton");
                }
                else if (entityId == "optionsButton" || entityId == "pauseOptionsButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeOptionsButton" : "unactiveOptionsButton");
                }
                else if (entityId == "tutorialButton" || entityId == "pauseTutorialButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeTutorialButton" : "unactiveTutorialButton");
                }
                else if (entityId == "confirmButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeConfirmButton" : "unactiveConfirmButton");
                }
                else if (entityId == "quitWindowButton" || entityId == "pauseQuitButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeQuitButton" : "unactiveQuitButton");
                }
                else if (entityId == "quitToMainMenuButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeYesButton" : "unactiveYesButton");
                }
                else if (entityId == "returnToPauseMenuButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeNoButton" : "unactiveNoButton");
                }
                else if (entityId == "nextLevelButton") {
                    updateButtons();
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeNextLevelButton" : "unactiveNextLevelButton");
                }
                else if (entityId == "mainMenuButton") {
                    ecsCoordinator.setTextureID(entity, entityId == hoveredButton ? "activeMainMenuButton" : "unactiveMainMenuButton");
                }
                else if (entityId == "closePauseMenu" || entityId == "closeOptionsMenu" || entityId == "closeTutorialMenu") {
                    ecsCoordinator.setTextureID(entity, "closePopupButton");
                }
                else if (entityId == "rotationSpeedSlider") {
                    ecsCoordinator.setTextureID(entity, "rotationSpeedSlider");
                }
                else if (entityId == "gameOverQuitButton") {
                    ecsCoordinator.setTextureID(entity, "buttonQuit");
                }
            }

            // Handle filter entities
            if (ecsCoordinator.hasComponent<FilterComponent>(entity)) {
                auto& filter = ecsCoordinator.getComponent<FilterComponent>(entity);
                if (filter.isFilterClogged) {
                    ecsCoordinator.setTextureID(entity, "filter_mossed");
                }
                else {
                    ecsCoordinator.setTextureID(entity, "exitFilter");
                }
            }

            // Handle enemy entities
            if (ecsCoordinator.hasComponent<EnemyComponent>(entity)) {
                auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);

                if (enemy.currState == 0) {
                    ecsCoordinator.setTextureID(entity, "goldfish");
                    auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                    enemyAnimation.totalFrames = 24;
                    enemyAnimation.columns = 4;
                    enemyAnimation.rows = 6;
                }
                else if (enemy.currState == 2) {
                    ecsCoordinator.setTextureID(entity, "goldfishBite");
                    auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
                    enemyAnimation.totalFrames = 16;
                    enemyAnimation.columns = 4;
                    enemyAnimation.rows = 4;
                }

                if (ecsCoordinator.getTextureID(entity) == "goldfishAlert") {
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

            // Handle one-time animations
            if (entityId == "collectAnimation" || entityId == "filterPush" ||
                entityId == "fishAlertAnimation" || entityId == "fishAttackAnimation") {
                auto& anim = ecsCoordinator.getComponent<AnimationComponent>(entity);

                double currentAbsoluteTime = glfwGetTime();
                double timeSinceCreation = currentAbsoluteTime - anim.creationTime;

                anim.currentFrame = static_cast<int>((timeSinceCreation / anim.frameTime)) % static_cast<int>(anim.totalFrames);

                if (anim.currentFrame == static_cast<int>(anim.totalFrames) - 1) {
                    ecsCoordinator.destroyEntity(entity);
                    continue;
                }
            }

            // Skip rendering if no texture assigned
            if (ecsCoordinator.getTextureID(entity).empty()) {
                continue;
            }

            // Skip filter-in animation when filter is clogged
            if (ecsCoordinator.getTextureID(entity) == "filter_in.png" && GLFWFunctions::filterClogged) {
                continue;
            }

            // Skip bubble animation if pump is not on
            if (ecsCoordinator.getTextureID(entity) == "bubbles 3.png" && !GLFWFunctions::isPumpOn) {
                continue;
            }

            // Draw debug information if debug flag is set
            if (GLFWFunctions::debug_flag && !ecsCoordinator.hasComponent<FontComponent>(entity) && !ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
                if (ecsCoordinator.hasComponent<ButtonComponent>(entity) || ecsCoordinator.hasComponent<UIComponent>(entity)) {
                    graphicsSystem.drawDebugOBB(ecsCoordinator.getComponent<TransformComponent>(entity), identityMatrix);
                }
                else {
                    graphicsSystem.drawDebugOBB(ecsCoordinator.getComponent<TransformComponent>(entity), viewMatrix);
                }
            }
            else if (GLFWFunctions::debug_flag && ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
                graphicsSystem.drawDebugCircle(ecsCoordinator.getComponent<TransformComponent>(entity), viewMatrix);
            }
        }

        // Now use batched rendering for the layer
        batchRender(i);
    }
}

// this is to update the level complete menu buttons in real-time
void GraphicSystemECS::updateButtons() {
    Entity nextLevelButton = ecsCoordinator.getEntityFromID("nextLevelButton");
    Entity mainMenuButton = ecsCoordinator.getEntityFromID("mainMenuButton");

    if (nextLevelButton == 0 || mainMenuButton == 0) return;

    TransformComponent& nextLevelTransform = ecsCoordinator.getComponent<TransformComponent>(nextLevelButton);
    TransformComponent& mainMenuTransform = ecsCoordinator.getComponent<TransformComponent>(mainMenuButton);

    if (GameViewWindow::getSceneNum() > 4) {
        nextLevelTransform.scale.SetX(0.f);
        nextLevelTransform.scale.SetY(0.f);
        mainMenuTransform.position.SetX(5.f);
    }
    else {
        nextLevelTransform.scale.SetX(260.f);
        nextLevelTransform.scale.SetY(130.f);
        mainMenuTransform.position.SetX(125.f);
    }
}

// this is to update the tutorial arrows scale in real-time
void GraphicSystemECS::updateTutorialArrows() {
    Entity nextArrow = ecsCoordinator.getEntityFromID("nextTutorialPage");
    Entity previousArrow = ecsCoordinator.getEntityFromID("previousTutorialPage");

    if (nextArrow == 0 || previousArrow == 0) return;

    TransformComponent& nextTransform = ecsCoordinator.getComponent<TransformComponent>(nextArrow);
    TransformComponent& previousTransform = ecsCoordinator.getComponent<TransformComponent>(previousArrow);

    if (GLFWFunctions::tutorialCurrentPage >= 1 && GLFWFunctions::tutorialCurrentPage < 8) {
        nextTransform.scale.SetX(100.f);
        nextTransform.scale.SetY(130.f);
    }
    else {
        nextTransform.scale.SetX(0.f);
        nextTransform.scale.SetY(0.f);
    }

    if (GLFWFunctions::tutorialCurrentPage > 1 && GLFWFunctions::tutorialCurrentPage <= 8) {
        previousTransform.scale.SetX(100.f);
        previousTransform.scale.SetY(130.f);
    }
    else {
        previousTransform.scale.SetX(0.f);
        previousTransform.scale.SetY(0.f);
    }
}

void GraphicSystemECS::cleanup() {
    sfxNotchEntities.clear();
    musicNotchEntities.clear();
    cachedEmptyUVs.clear();
    entityTypeMap.clear();
}

std::string GraphicSystemECS::getSystemECS() {
    return "GraphicsSystemECS";
}