/*
All content @ 2025 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  CutsceneSystem.cpp
@brief  :  This file contains the implementation of the CutsceneSystem class. It is
           responsible for managing the cutscene's camera position,
           rotation, and zoom, as well as updating the view matrix based on these values.

 File Contributions: Liu YaoTing (100%)

/*_______________________________________________________________________________________________________________*/


#include "CutsceneSystem.h"
#include "GlobalCoordinator.h"
#include "GlfwFunctions.h"

 std::vector<CutsceneSystem::CutsceneFrame>* CutsceneSystem::m_frames;

CutsceneSystem::CutsceneSystem() : m_currentFrameIndex(0), m_currentFrameTime(0.0f), m_isPlaying(false), m_originalZoom(1.0f) {}

void CutsceneSystem::initialise() {
    // Reset all cutscene data to initial state
    if (!m_frames) {
		m_frames = new std::vector<CutsceneFrame>();
    }
    m_frames->clear();
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;
    m_isPlaying = false;
}

void CutsceneSystem::update() {
    // Skip to end if ESCAPE is held
    if ((*GLFWFunctions::keyState)[Key::ENTER]) {
        skipToEnd();
        return;
    }

    if (!m_isPlaying || m_currentFrameIndex >= m_frames->size()) {
        if (m_isPlaying) {
            cameraSystem.setCameraZoom(m_originalZoom);
            m_isPlaying = false;
        }
        return;
    }
    if (m_currentFrameIndex >= m_frames->size()) {
        m_isPlaying = false;
        // Reset camera position to origin when cutscene completes
        cameraSystem.setCameraPosition(myMath::Vector2D(0.0f, 0.0f));
        return;
    }

    // Next frame on SPACE press
    static bool spaceWasPressed = false;
    if ((*GLFWFunctions::keyState)[Key::SPACE]) {
        if (!spaceWasPressed) {
            m_currentFrameIndex++;
            m_currentFrameTime = 0.0f;
            spaceWasPressed = true;

            if (m_currentFrameIndex >= m_frames->size()) {
                m_isPlaying = false;
                return;
            }
        }
    }
    else {
        spaceWasPressed = false;
    }

	auto& currentFrame = m_frames->at(m_currentFrameIndex);
    m_currentFrameTime += GLFWFunctions::delta_time;

    // Calculate transition progress (0 to 1)
    float t = m_currentFrameTime / currentFrame.duration;

    // Get start and target positions
    myMath::Vector2D startPos = (m_currentFrameIndex == 0) ?
        currentFrame.cameraPosition :
		m_frames->at(m_currentFrameIndex - 1).cameraPosition;
    myMath::Vector2D targetPos = currentFrame.cameraPosition;

    // Interpolate between positions
    myMath::Vector2D newPos = lerp(startPos, targetPos, t);
    cameraSystem.setCameraPosition(newPos);

    if (m_currentFrameTime >= currentFrame.duration) {
        currentFrame.hasCompleted = true;
        m_currentFrameIndex++;
        m_currentFrameTime = 0.0f;

        if (m_currentFrameIndex >= m_frames->size()) {
            m_isPlaying = false;
        }
    }
}

void CutsceneSystem::cleanup() {
    delete m_frames;
    m_frames = nullptr;
    m_isPlaying = false;
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;
}

SystemType CutsceneSystem::getSystem() {
    return SystemType::CutsceneSystemType;
}

void CutsceneSystem::addFrame(const myMath::Vector2D& position, float duration) {
    m_frames->emplace_back(position, duration);
}

void CutsceneSystem::start() {
    if (m_frames->empty()) {
        return;
    }

    m_isPlaying = true;
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;

    // Unlock camera from any entities
    cameraSystem.unlockFromComponent();

    // Store original zoom
    m_originalZoom = cameraSystem.getCameraZoom();

    // Set appropriate zoom level for cutscene
	if (GLFWFunctions::fullscreen == true)
		cameraSystem.setCameraZoom(1.5f);
	else if (GLFWFunctions::fullscreen == false)
        cameraSystem.setCameraZoom(1.3f);

    // Set initial camera position 
	cameraSystem.setCameraPosition(m_frames->at(0).cameraPosition);
}

void CutsceneSystem::stop() {
    m_isPlaying = false;

    // Restore original zoom
    cameraSystem.setCameraZoom(m_originalZoom);
}

bool CutsceneSystem::isFinished() const {
    return !m_isPlaying && m_currentFrameIndex >= m_frames->size();
}

void CutsceneSystem::skipToEnd() {
    if (!m_frames->empty()) {
        // Move camera to final position
        cameraSystem.setCameraPosition(m_frames->back().cameraPosition);
    }

    // Mark cutscene as complete
    m_currentFrameIndex = m_frames->size();
    m_isPlaying = false;
    cameraSystem.setCameraPosition(myMath::Vector2D(0.0f, 0.0f));
}

myMath::Vector2D CutsceneSystem::lerp(const myMath::Vector2D& start, const myMath::Vector2D& end, float t) {
    // Smooth the interpolation using easing function
    t = t * t * (3 - 2 * t); // Smoothstep formula

    return myMath::Vector2D(
        start.GetX() + (end.GetX() - start.GetX()) * t,
        start.GetY() + (end.GetY() - start.GetY()) * t
    );
}