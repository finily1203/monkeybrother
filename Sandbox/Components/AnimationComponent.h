/*! All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 * @author: Javier Chua (javierjunliang.chua)
 * @team: MonkeHood
 * @course: CSD2401
 * @file: AnimationComponent.h
 * @brief: This header file includes the Animation Component to be used by ECS
 *         and physics and collision System to handle the logic of the game objects.
 */
 /*___________________________________________________________________________*/

#pragma once

#include "AnimationData.h"

struct AnimationConfig {
    float columns;
    float rows;
    float totalFrames;
    std::string textureName;
};


struct MovementAnimConfig {
    float movementThreshold = 5.0f;
    const char* bodyTexture = "mossball_move_body";
    const char* eyesTexture = "mossball_move_eyes";
    float bodyFrames = 24.0f;
    float bodyColumns = 8.0f;
    float bodyRows = 3.0f;
    float eyesFrames = 16.0f;
    float eyesColumns = 8.0f;
    float eyesRows = 2.0f;
    float eyeFrameDuration = 0.1f;
};


struct GrowthAnimationConfig {
    AnimationConfig body = { 8.0f, 2.0f, 16.0f, "mossball_grow_body" };
    AnimationConfig eyes = { 8.0f, 2.0f, 16.0f, "mossball_grow_eyes" };
    float duration = 1.0f; // Duration of the growth animation in seconds
};


struct IdleAnimationConfig {
    AnimationConfig body = { 4.0f, 7.0f, 26.0f, "mossball_idle_bodyrecentre_start" };
    AnimationConfig eyes = { 4.0f, 5.0f, 18.0f, "mossball_idle_eyesclose" };
    float duration = 1.5f; // Duration of the idle animation
};


struct AnimationComponent {
    double creationTime;
    bool isAnimated;
    int currentFrame;
    float totalFrames;
    float frameTime;
    float currentTime;
    float columns;
    float rows;
    std::vector<glm::vec2> currentUVs;

    AnimationComponent()
        : isAnimated(false)
        , currentFrame(0)
        , totalFrames(1.0f)
        , frameTime(0.0f)
        , currentTime(0.0f)
        , columns(1.0f)
        , rows(1.0f)
        , creationTime(glfwGetTime())
    {
        currentUVs.resize(4);
        UpdateUVCoordinates();
    }

    AnimationComponent(float frames, float time, float cols, float rws)
        : isAnimated(false)
        , currentFrame(0)
        , totalFrames(std::max(1.0f, frames))
        , frameTime(std::max(0.001f, time))
        , currentTime(0.0f)
        , columns(std::max(1.0f, cols))
        , rows(std::max(1.0f, rws))
    {
        currentUVs.resize(4);
        UpdateUVCoordinates();
    }

    void Update() {
        if (!isAnimated || totalFrames <= 1.0f) return;


        double currentAbsoluteTime = glfwGetTime();
        double timeSinceCreation = currentAbsoluteTime - creationTime;

        currentFrame = static_cast<int>((timeSinceCreation / frameTime)) % static_cast<int>(totalFrames);

        UpdateUVCoordinates();
    }

    void UpdateUVCoordinates() {


        // Calculate frame size in UV coordinates
        float frameWidth = 1.0f / columns;
        float frameHeight = 1.0f / rows;

        // Calculate current frame position
        int currentRow = currentFrame / static_cast<int>(columns);
        int currentCol = currentFrame % static_cast<int>(columns);

        // Calculate UV coordinates for current frame
        float uMin = frameWidth * currentCol;
        float uMax = uMin + frameWidth;
        float vMax = 1.0f - (frameHeight * currentRow);
        float vMin = vMax - frameHeight;


        currentUVs[0] = glm::vec2(uMax, vMax);  // Top right
        currentUVs[1] = glm::vec2(uMax, vMin);  // Bottom right
        currentUVs[2] = glm::vec2(uMin, vMin);  // Bottom left
        currentUVs[3] = glm::vec2(uMin, vMax);  // Top left
    }
};
