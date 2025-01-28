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
    bool isAnimated;           
    int currentFrame;          
    float totalFrames;         
    float frameTime;           
    float currentTime;         
    float columns;            
    float rows;               

    AnimationComponent()
        : isAnimated(false)
        , currentFrame(0)
        , totalFrames(1.0f)
        , frameTime(0.0f)
        , currentTime(0.0f)
        , columns(0.0f)       
        , rows(0.0f)         
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


    void Update(float deltaTime) {
        if (!isAnimated || totalFrames <= 1.0f) return;

        currentTime += deltaTime;
        if (currentTime >= frameTime) {
            currentTime -= frameTime;
       
            currentFrame = static_cast<int>((currentFrame + 1) % static_cast<int>(totalFrames));
        }
    }

    
    int GetColumns() const { return static_cast<int>(columns); }
    int GetRows() const { return static_cast<int>(rows); }

    ~AnimationComponent() {
        
        currentFrame = 0;
        currentTime = 0.0f;
    }
};