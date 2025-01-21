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
#include <memory> // for std::unique_ptr
#include "AnimationData.h"

struct AnimationComponent {
    bool isAnimated;           // Whether the entity is animated
    int currentFrame;          // Current frame for animation display
    float totalFrames;         // Total number of frames in animation as float
    float frameTime;           // Time per frame
    float currentTime;         // Time accumulated since last frame change
    float columns;            // Number of columns in sprite sheet as float
    float rows;               // Number of rows in sprite sheet as float

    AnimationComponent()
        : isAnimated(false)
        , currentFrame(0)
        , totalFrames(1.0f)
        , frameTime(0.0f)
        , currentTime(0.0f)
        , columns(0.0f)       // Default to 4 columns
        , rows(0.0f)         // Default to 6 rows
    {}

    // Constructor with animation parameters
    AnimationComponent(float frames, float time, float cols, float rws)
        : isAnimated(true)
        , currentFrame(0)
        , totalFrames(frames)
        , frameTime(time)
        , currentTime(0.0f)
        , columns(cols)
        , rows(rws)
    {}

    // Update animation state using float totalFrames
    void Update(float deltaTime) {
        if (!isAnimated || totalFrames <= 1.0f) return;

        currentTime += deltaTime;
        if (currentTime >= frameTime) {
            currentTime -= frameTime;
            // Compute the next frame by wrapping within the total frame count
            currentFrame = static_cast<int>((currentFrame + 1) % static_cast<int>(totalFrames));
        }
    }

    // Helper methods to get integer values of rows and columns
    int GetColumns() const { return static_cast<int>(columns); }
    int GetRows() const { return static_cast<int>(rows); }

    ~AnimationComponent() {
        // Clean up any resources if needed
        currentFrame = 0;
        currentTime = 0.0f;
    }
};