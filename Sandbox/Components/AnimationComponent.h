/*! All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 * @author: Joel Chu (c.weiyuan)
 * @team: MonkeHood
 * @course: CSD2401
 * @file: AnimationComponent.h
 * @brief: This header file includes the Animation Component to be used by ECS
 *         and physics and collision System to handle the logic of the game objects.
 */
 /*___________________________________________________________________________*/

#pragma once

#include "AnimationData.h"

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
        UpdateUVCoordinates(); // Initialize with proper UV coordinates
    }

    AnimationComponent(float frames, float time, float cols, float rws)
        : isAnimated(true)
        , currentFrame(0)
        , totalFrames(std::max(1.0f, frames))
        , frameTime(std::max(0.001f, time))
        , currentTime(0.0f)
        , columns(std::max(1.0f, cols))
        , rows(std::max(1.0f, rws))
    {
        currentUVs.resize(4);
        UpdateUVCoordinates(); // Initialize with proper UV coordinates
    }

    void Update() {
        if (!isAnimated || totalFrames <= 1.0f) return;

        // Use absolute time since creation for animation
        double currentAbsoluteTime = glfwGetTime();
        double timeSinceCreation = currentAbsoluteTime - creationTime;

        // Calculate current frame based on absolute time
        currentFrame = static_cast<int>((timeSinceCreation / frameTime)) % static_cast<int>(totalFrames);

        UpdateUVCoordinates();
    }

    void UpdateUVCoordinates() {
        if (!isAnimated) {
            // For non-animated sprites, use the entire texture
            currentUVs[0] = glm::vec2(1.0f, 1.0f);  // Top right
            currentUVs[1] = glm::vec2(1.0f, 0.0f);  // Bottom right
            currentUVs[2] = glm::vec2(0.0f, 0.0f);  // Bottom left
            currentUVs[3] = glm::vec2(0.0f, 1.0f);  // Top left
            return;
        }

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

        // Assign UV coordinates for the quad
        currentUVs[0] = glm::vec2(uMax, vMax);  // Top right
        currentUVs[1] = glm::vec2(uMax, vMin);  // Bottom right
        currentUVs[2] = glm::vec2(uMin, vMin);  // Bottom left
        currentUVs[3] = glm::vec2(uMin, vMax);  // Top left
    }
};
