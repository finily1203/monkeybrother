#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <string>
#include <map>
#include <iostream>

class AnimationData {
public:
    AnimationData(int totalFrames, float frameDuration, int columns, int rows);

    void Update(float deltaTime);
    void SetCurrentAction(int action);
    void UpdateUVCoordinates();

   
    inline const std::vector<glm::vec2>& GetCurrentUVs() const {
        return currentUVs;
    }

    inline int GetCurrentFrame() const {
        return currentFrame;
    }

    inline int GetCurrentAction() const {
        return currentAction;
    }

    void ResetAnimation();

    void SetFrameDuration(float duration);
    void SetSpeedMultiplier(float multiplier);
    void SetLooping(bool shouldLoop);

    void AddFrameEvent(int frame, const std::string& eventName);
    void TriggerFrameEvents();

private:
    
    int totalFrames;
    int columns;
    int rows;
    int framesPerAction;
    int currentFrame;
    int currentAction;
    float timeAccumulator;
    float frameDuration;
    float speedMultiplier;
    bool looping;
    bool uvDirty;

    float frameWidth;
    float frameHeight;

    std::vector<glm::vec2> currentUVs;
    std::map<int, std::vector<std::string>> frameEvents;
};
