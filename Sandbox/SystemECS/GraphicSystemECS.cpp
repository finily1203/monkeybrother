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
				bool isExit = ecsCoordinator.hasComponent<ExitComponent>(entity);

                // Use hasMovement for the update parameter
                //graphicsSystem.Update(dt / 10.0f, isAnimate || hasMovement || isEnemy);


                //graphicsSystem.Update(dt / 10.0f, (isAnimate&& isPump) || (isPlayer && hasMovement) || (isEnemy && hasMovement)); // Use hasMovement instead of true
                myMath::Matrix3x3 identityMatrix = { 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f };
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, cameraSystem.getViewMatrix());


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

                if (ecsCoordinator.getEntityID(entity) == "sfxAudio" || ecsCoordinator.getEntityID(entity) == "musicAudio")
                {
                    std::string audioType = ecsCoordinator.getEntityID(entity);
                    TransformComponent arrowTransform{}, soundbarTransform{};

                    std::string audioArrowId = (audioType == "sfxAudio") ? "sfxSoundbarArrow" :
                        (audioType == "musicAudio") ? "musicSoundbarArrow" : "";

            std::string soundbarId = (audioType == "sfxAudio") ? "sfxSoundbarBase" :
                (audioType == "musicAudio") ? "musicSoundbarBase" : "";

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

                float soundbarLeftBoundary = soundbarTransform.position.GetX() - (soundbarTransform.scale.GetX() / 2.15f);

                std::string textureName = (arrowTransform.position.GetX() <= soundbarLeftBoundary) ? "soundMute" : "soundOn";
                transform.mdl_xform = graphicsSystem.UpdateObject(transform.position, transform.scale, transform.orientation, identityMatrix);
                ecsCoordinator.setTextureID(entity, textureName);
                //graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(textureName), transform.mdl_xform);
            }
        }

        if (ecsCoordinator.getEntityID(entity).find("sfxNotch") != std::string::npos ||
            ecsCoordinator.getEntityID(entity).find("musicNotch") != std::string::npos)
        {
            int activeNotchesSFX{}, activeNotchesMusic{};
            float startPosSFX{}, endPosSFX{};
            float startPosMusic{}, endPosMusic{};

            TransformComponent sfxArrowTransform{}, musicArrowTransform{};

            // Identify arrow positions for SFX and Music
            for (auto& arrowEntity : ecsCoordinator.getAllLiveEntities())
            {
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

            std::vector<std::pair<Entity, TransformComponent>> sfxNotches, musicNotches;

            // Separate SFX and Music Notches
            for (auto& notchEntity : ecsCoordinator.getAllLiveEntities())
            {
                std::string notchId = ecsCoordinator.getEntityID(notchEntity);
                TransformComponent notchTransform = ecsCoordinator.getComponent<TransformComponent>(notchEntity);

                if (notchId.find("sfxNotch") != std::string::npos)
                {
                    sfxNotches.emplace_back(notchEntity, notchTransform);
                }

                else if (notchId.find("musicNotch") != std::string::npos)
                {
                    musicNotches.emplace_back(notchEntity, notchTransform);
                }
            }

            float sfxPercentage{}, musicPercentage{};

            if (sfxNotches.size() == 10)
            {
                startPosSFX = sfxNotches[0].second.position.GetX() - (sfxNotches[0].second.scale.GetX() / 2.0f);
                endPosSFX = sfxNotches[9].second.position.GetX() + (sfxNotches[9].second.scale.GetX() / 2.0f);

                float arrowPosSFX = sfxArrowTransform.position.GetX();
                float progressSFX = std::abs((arrowPosSFX - startPosSFX) / (endPosSFX - startPosSFX));
                if (progressSFX >= 0.01f && progressSFX <= 0.09f) {
                    sfxPercentage = 10.0f;
                }
				else {
					sfxPercentage = std::round(progressSFX * 10.f) * 10.f;
                }
                sfxPercentage = std::clamp(sfxPercentage, 0.f, 100.f);
                AudioSystem::sfxPercentage = sfxPercentage;
            }

            if (musicNotches.size() == 10)
            {
                startPosMusic = musicNotches[0].second.position.GetX() - (musicNotches[0].second.scale.GetX() / 2.0f);
                endPosMusic = musicNotches[9].second.position.GetX() + (musicNotches[9].second.scale.GetX() / 2.0f);

                float arrowPosMusic = musicArrowTransform.position.GetX();
                float progressMusic = std::abs((arrowPosMusic - startPosMusic) / (endPosMusic - startPosMusic));
				if (progressMusic >= 0.01f && progressMusic <= 0.09f) {
					musicPercentage = 10.0f;
				}
                else {
                    musicPercentage = std::round(progressMusic * 10.f) * 10.f; 
                }
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

                if (isExit) {
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

                    else if (ecsCoordinator.getEntityID(entity) == "closePauseMenu" || ecsCoordinator.getEntityID(entity) == "closeOptionsMenu")
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
                }

                //if is player, check visibility of player, if not visible do not render
                if (isPlayer) {
                    auto& player = ecsCoordinator.getComponent<PlayerComponent>(entity);
                    if (!player.isVisible) {
                        continue;
                    }
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

                if (ecsCoordinator.getEntityID(entity) == "collectAnimation") {
                    auto& anim = ecsCoordinator.getComponent<AnimationComponent>(entity);

                    double currentAbsoluteTime = glfwGetTime();
                    double timeSinceCreation = currentAbsoluteTime - anim.creationTime;

                    anim.currentFrame = static_cast<int>((timeSinceCreation / anim.frameTime)) % static_cast<int>(anim.totalFrames);

                    // Check if one loop is completed
                    if (anim.currentFrame == static_cast<int>(anim.totalFrames) - 1) {

                        // Optional: Trigger actions after one loop
                        ecsCoordinator.destroyEntity(entity);  // Example: Destroy after one loop
                    }
                }

                if (ecsCoordinator.getTextureID(entity) != "") {
                    //should not render the animation for filter in and filter out if filter is clogged
                    if (ecsCoordinator.getTextureID(entity) == "filter_in.png" || ecsCoordinator.getTextureID(entity) == "filter_out.png") {
                        if (GLFWFunctions::filterClogged) {
                            continue;
                        }
                    }
                    //should not render the bubble animation if pump is not on
                    if (ecsCoordinator.getTextureID(entity) == "bubbles 3.png") {
                        if (!GLFWFunctions::isPumpOn) {
                            continue;
                        }
                    }
                    graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture(ecsCoordinator.getTextureID(entity)), transform.mdl_xform, animation.currentUVs);
                }

				//std::cout << "isPump on? " << (GLFWFunctions::isPumpOn ? "true" : "false") << std::endl;

                /*if (isPlayer) {
                    graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("eyes.png"), transform.mdl_xform, animation.currentUVs);
                }*/
            }
        }
    }
}



void GraphicSystemECS::cleanup() {}

std::string GraphicSystemECS::getSystemECS() {
    return "GraphicsSystemECS";
}