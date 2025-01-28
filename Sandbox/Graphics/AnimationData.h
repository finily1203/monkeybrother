/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  AnimationData.h
@brief  :  This file contains the declaration of the AnimationData class, which
           is responsible for managing the animation data of a game object.
*
* Javier Chua (javierjunliang.chua) :
*       - Implemented the AnimationData class, which is responsible for managing the
          animation data of a game object.
*
* File Contributions: Javier Chua (100%)
*
/*_ _ _ _ ________________________________________________________________________________-\*/
#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <string>
#include <map>
#include <iostream>

enum class Direction {
    Left,
    Right
};
class AnimationData {
public:
	// Constructor
    AnimationData(int totalFrames, float frameDuration, int columns, int rows);
    ~AnimationData();
	// Update function
    void Update(float deltaTime);
	// Update UV coordinates
    void UpdateUVCoordinates();
	// Get current UVs
    inline const std::vector<glm::vec2>& GetCurrentUVs() const {
        return *currentUVs;
    }
	// Get current frame
    inline int GetCurrentFrame() const {
        return currentFrame;
    }
	// Get current action
    inline int GetCurrentAction() const {
        return currentAction;
    }
	// Set current action
    void ResetAnimation();
	// Set current action
    void SetFrameDuration(float duration);
	// Set current action
    void SetSpeedMultiplier(float multiplier);
	// Set current action
    void SetLooping(bool shouldLoop);
	// Set current action
    void AddFrameEvent(int frame, const std::string& eventName);
	// Set current action
    void TriggerFrameEvents();
    void SetAnimationTotalFrames(int newTotalFrames);
    void SetAnimationFrameDuration(float newFrameDuration);
    void SetAnimationColumns(int newColumns);
    void SetAnimationRows(int newRows);
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
    Direction lastDirection;
    std::vector<glm::vec2> *currentUVs;
    std::map<int, std::vector<std::string>> frameEvents;

};
