#include "AnimationData.h"
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <map>

AnimationData::AnimationData(int totalFrames, float frameDuration, int columns, int rows)
    : totalFrames(totalFrames), frameDuration(frameDuration), columns(columns), rows(rows),
    currentFrame(0), currentAction(0), timeAccumulator(0.0f), uvDirty(true),
    speedMultiplier(1.0f), looping(true) {
    // Validate inputs
    if (totalFrames <= 0 || columns <= 0 || rows <= 0) {
        throw std::invalid_argument("Total frames, columns, and rows must be greater than zero.");
    }

    // Precompute the width and height for each frame in UV space
    frameWidth = 1.0f / columns;
    frameHeight = 1.0f / rows;

    // Calculate frames per action
    framesPerAction = totalFrames / rows;

    // Initialize UV coordinates for the quad
    currentUVs.resize(4);
    UpdateUVCoordinates();  // Set initial UVs based on the first frame and action
}

void AnimationData::Update(float deltaTime) {
    if (deltaTime < 0.0f) {
        throw std::invalid_argument("Delta time cannot be negative.");
    }

    timeAccumulator += deltaTime * speedMultiplier;  // Apply speed multiplier

    if (timeAccumulator >= frameDuration) {
        timeAccumulator = 0.0f;  // Reset time accumulator
        currentFrame = (currentFrame + 1) % framesPerAction; // Loop within current action's frames

      
        if (!looping && currentFrame == 0) {
            currentFrame = framesPerAction - 1;  // Stay at last frame if not looping
        }

        uvDirty = true;  // Mark UVs for update since frame has changed
    }

    if (uvDirty) {
        UpdateUVCoordinates();
        uvDirty = false;
    }
}

void AnimationData::SetCurrentAction(int action) {
    if (action < 0 || action >= rows) {
        throw std::out_of_range("Action index out of range.");
    }

    if (action != currentAction) {
        currentAction = action;
        currentFrame = 0;  // Reset to the first frame of the new action
        uvDirty = true;   
    }
}

void AnimationData::UpdateUVCoordinates() {
    float uMin = frameWidth * currentFrame;          // Left side of the frame (U)
    float uMax = uMin + frameWidth;                  // Right side of the frame (U)
    float vMin = 1.0f - frameHeight * (currentAction + 1);  // Top of the frame (V), flipped vertically
    float vMax = vMin + frameHeight;                 // Bottom of the frame (V)

    // Define UV coordinates for a quad (4 vertices)
    currentUVs[0] = glm::vec2(uMax, vMax);  // Top right
    currentUVs[1] = glm::vec2(uMax, vMin);  // Bottom right
    currentUVs[2] = glm::vec2(uMin, vMin);  // Bottom left
    currentUVs[3] = glm::vec2(uMin, vMax);  // Top left
}


void AnimationData::ResetAnimation() {
    currentFrame = 0;
    currentAction = 0;
    timeAccumulator = 0.0f;
    uvDirty = true;
    UpdateUVCoordinates();
}

void AnimationData::SetFrameDuration(float duration) {
    if (duration <= 0.0f) {
        throw std::invalid_argument("Frame duration must be greater than zero.");
    }
    frameDuration = duration;
}

void AnimationData::SetSpeedMultiplier(float multiplier) {
    if (multiplier <= 0.0f) {
        throw std::invalid_argument("Speed multiplier must be greater than zero.");
    }
    speedMultiplier = multiplier;
}

void AnimationData::SetLooping(bool shouldLoop) {
    looping = shouldLoop;
}

void AnimationData::AddFrameEvent(int frame, const std::string& eventName) {
    if (frame < 0 || frame >= framesPerAction) {
        throw std::out_of_range("Frame index out of range.");
    }
    frameEvents[frame].push_back(eventName);  // Store event names for the specific frame
}

void AnimationData::TriggerFrameEvents() {
    auto eventsIt = frameEvents.find(currentFrame);
    if (eventsIt != frameEvents.end()) {

    }
}


