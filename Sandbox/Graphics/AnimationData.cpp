/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  AnimationData.cpp
@brief  :  This file contains the implementation of the AnimationData class, which
           is responsible for managing the animation data of a game object.
*
* Javier Chua (javierjunliang.chua) :
*       - Implemented the AnimationData class, which is responsible for managing the
          animation data of a game object.
*         This class is used to store the total number of frames, frame duration, number
          of columns and rows, current frame,
*
* File Contributions: Javier Chua (100%)
*
/*_ _ _ _ ________________________________________________________________________________-\*/

#include "AnimationData.h"
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <map>
#include "GlobalCoordinator.h"

// Constructor implementation
AnimationData::AnimationData(int totalFrames, float frameDuration, int columns, int rows)
    : totalFrames(totalFrames), frameDuration(frameDuration), columns(columns), rows(rows),
    currentFrame(0), timeAccumulator(0.0f), uvDirty(true),
    speedMultiplier(1.0f), looping(true), lastDirection(Direction::Right) {

    currentUVs = new std::vector<glm::vec2>();

    // Validate inputs
    if (totalFrames <= 0 || columns <= 0 || rows <= 0) {
        throw std::invalid_argument("Total frames, columns, and rows must be greater than zero.");
    }


    frameWidth = 1.0f / columns;
    frameHeight = 1.0f / rows;


    currentUVs->resize(4);
    UpdateUVCoordinates();
}

AnimationData::~AnimationData() {
    delete currentUVs;
    currentUVs = nullptr;
 
    frameEvents.clear();
    std::map<int, std::vector<std::string>>().swap(frameEvents);
}
// Update function implementation
void AnimationData::Update(float deltaTime) {
    if (deltaTime < 0.0f) {
        throw std::invalid_argument("Delta time cannot be negative.");
    }

    timeAccumulator += deltaTime;

    if (timeAccumulator >= frameDuration) {
        timeAccumulator -= frameDuration;

        // Always loop through the first 14 frames
        currentFrame = (currentFrame + 1) % 24;

        uvDirty = true;
    }

    if (uvDirty) {
        UpdateUVCoordinates();
        uvDirty = false;
    }
}


// pdate UV coordinates function implementation
void AnimationData::UpdateUVCoordinates() {

    float uMin = frameWidth * (currentFrame % columns);
    float uMax = uMin + frameWidth;
    float vMin = 1.0f - frameHeight * ((currentFrame / columns) + 1);
    float vMax = vMin + frameHeight;
    // Define UV coordinates for a quad (4 vertices)
    (*currentUVs)[0] = glm::vec2(uMax, vMax);  // Top right
    (*currentUVs)[1] = glm::vec2(uMax, vMin);  // Bottom right
    (*currentUVs)[2] = glm::vec2(uMin, vMin);  // Bottom left
    (*currentUVs)[3] = glm::vec2(uMin, vMax);  // Top left
}

// Reset animation function implementation
void AnimationData::ResetAnimation() {
    currentFrame = 0;
    timeAccumulator = 0.0f;
    uvDirty = true;
    UpdateUVCoordinates();
}

// Set frame duration function implementation
void AnimationData::SetFrameDuration(float duration) {
    if (duration <= 0.0f) {
        throw std::invalid_argument("Frame duration must be greater than zero.");
    }
    frameDuration = duration;
}

// Set speed multiplier function implementation
void AnimationData::SetSpeedMultiplier(float multiplier) {
    if (multiplier <= 0.0f) {
        throw std::invalid_argument("Speed multiplier must be greater than zero.");
    }
    speedMultiplier = multiplier;
}

// Set looping function implementation
void AnimationData::SetLooping(bool shouldLoop) {
    looping = shouldLoop;
}

// Add frame event function implementation
void AnimationData::AddFrameEvent(int frame, const std::string& eventName) {
    if (frame < 0 || frame >= totalFrames) {
        throw std::out_of_range("Frame index out of range.");
    }
    frameEvents[frame].push_back(eventName);  
}

// Trigger frame events function implementation
void AnimationData::TriggerFrameEvents() {
    auto eventsIt = frameEvents.find(currentFrame);
    if (eventsIt != frameEvents.end()) {

    }
}


