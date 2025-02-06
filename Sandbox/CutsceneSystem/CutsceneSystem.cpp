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
#include "GUIGameViewport.h"

std::vector<CutsceneSystem::CutsceneFrame>* CutsceneSystem::m_frames;
bool hasZoomBeenSet = false;

CutsceneSystem::CutsceneSystem()
    : m_currentFrameIndex(0), m_currentFrameTime(0.0f), m_isPlaying(false), m_originalZoom(1.0f) {
}

void CutsceneSystem::initialise() {
    if (!m_frames) {
        m_frames = new std::vector<CutsceneFrame>();
    }
    m_frames->clear();
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;
    m_isPlaying = false;
}

void CutsceneSystem::update() {
    // Update delay timer
    if (m_delayTimer < m_initialDelay) {
        m_delayTimer += GLFWFunctions::delta_time;
    }
    // Skip to end if ENTER is held
    if ((*GLFWFunctions::keyState)[Key::ENTER]) {
        skipToEnd();
        return;
    }

    if (!m_isPlaying || m_currentFrameIndex >= m_frames->size()) {
        if (!hasZoomBeenSet) {
            cameraSystem.setCameraZoom(0.2f);
            hasZoomBeenSet = true;
        }
        return;
    }

    if (m_currentFrameIndex >= m_frames->size()) {
        m_isPlaying = false;
        cameraSystem.setCameraPosition(myMath::Vector2D(0.0f, 0.0f));
        return;
    }

    // Check for space press or left mouse click to advance frame
    static bool spaceWasPressed = false;
    static bool mouseWasClicked = false;
    bool shouldAdvance = false;

    // Check space key
    if ((*GLFWFunctions::keyState)[Key::SPACE]) {
        if (!spaceWasPressed && canAcceptInput()) {
            shouldAdvance = true;
            spaceWasPressed = true;
        }
    }
    else {
        spaceWasPressed = false;
    }

    // Only check for mouse clicks if we're in the cutscene scene AND past the initial delay
    if (GameViewWindow::getSceneNum() == -2 && canAcceptInput()) {
        // Check left mouse button
        if (GLFWFunctions::mouseButtonState &&
            (*GLFWFunctions::mouseButtonState)[MouseButton::left]) {
            if (!mouseWasClicked) {
                shouldAdvance = true;
                mouseWasClicked = true;
            }
        }
        else {
            mouseWasClicked = false;
        }
    }

    auto& currentFrame = m_frames->at(m_currentFrameIndex);

    if (shouldAdvance) {
        m_currentFrameIndex++;
        m_currentFrameTime = 0.0f;

        if (m_currentFrameIndex >= m_frames->size()) {
            m_isPlaying = false;
            return;
        }
    }

    // Calculate transition progress (0 to 1)
    float t = m_currentFrameTime / currentFrame.duration;
    t = std::min(t, 1.0f); // Clamp to prevent overshooting

    // Get start and target positions
    myMath::Vector2D startPos = (m_currentFrameIndex == 0) ?
        currentFrame.cameraPosition :
        m_frames->at(m_currentFrameIndex - 1).cameraPosition;
    myMath::Vector2D targetPos = currentFrame.cameraPosition;

    // Get start and target zoom levels
    float startZoom = (m_currentFrameIndex == 0) ?
        currentFrame.zoom :
        m_frames->at(m_currentFrameIndex - 1).zoom;
    float targetZoom = currentFrame.zoom;

    // Interpolate position and zoom
    myMath::Vector2D newPos = lerp(startPos, targetPos, t);

    if (newPos.GetX() == targetPos.GetX() && newPos.GetY() == targetPos.GetY()) {
		currentFrame.hasCompleted = true;
    }

    float newZoom = startZoom + (targetZoom - startZoom) * t;

    // Apply transitions
    cameraSystem.setCameraPosition(newPos);
    if (GLFWFunctions::fullscreen) {
        cameraSystem.setCameraZoom(newZoom);
    }
    else {
        cameraSystem.setCameraZoom(static_cast<GLfloat>(0.85 * newZoom));
    }

    // Update time and check for frame completion
    if (t < 1.0f) {
        m_currentFrameTime += GLFWFunctions::delta_time;
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

void CutsceneSystem::addFrame(const myMath::Vector2D& position, float zoom, float duration) {
    m_frames->emplace_back(position, zoom, duration);
}

void CutsceneSystem::start() {
    if (m_frames->empty()) {
        return;
    }

    m_isPlaying = true;
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;
    m_delayTimer = 0.0f;  // Reset the delay timer

    // Unlock camera from any entities
    cameraSystem.unlockFromComponent();

    // Store original zoom
    m_originalZoom = cameraSystem.getCameraZoom();

    // Set initial position and zoom
    cameraSystem.setCameraPosition(m_frames->at(0).cameraPosition);
    cameraSystem.setCameraZoom(m_frames->at(0).zoom);
}

void CutsceneSystem::stop() {
    m_isPlaying = false;
    cameraSystem.setCameraZoom(m_originalZoom);
}

bool CutsceneSystem::isPlaying() const {
    return m_isPlaying;
}

bool CutsceneSystem::isFinished() const {
    return !m_isPlaying && m_currentFrameIndex >= m_frames->size();
}

void CutsceneSystem::skipToEnd() {
    if (!m_frames->empty()) {
        // Move camera to final position and zoom
        cameraSystem.setCameraPosition(m_frames->back().cameraPosition);
        cameraSystem.setCameraZoom(m_frames->back().zoom);
    }

    // Mark cutscene as complete
    m_currentFrameIndex = m_frames->size();
    m_isPlaying = false;
    cameraSystem.setCameraPosition(myMath::Vector2D(0.0f, 0.0f));
    cameraSystem.setCameraZoom(0.2f);
}

size_t CutsceneSystem::getCurrentFrameIndex() const {
	return m_currentFrameIndex;
}

bool CutsceneSystem::getFrameCompletion(size_t index) const {
    if (index < m_frames->size() && index >= 0) {
        return m_frames->at(index).hasCompleted;
    }
    return false;
}

myMath::Vector2D CutsceneSystem::lerp(const myMath::Vector2D& start, const myMath::Vector2D& end, float t) {
    // Smooth the interpolation using easing function
    t = t * t * (3 - 2 * t); // Smoothstep formula

    return myMath::Vector2D(
        start.GetX() + (end.GetX() - start.GetX()) * t,
        start.GetY() + (end.GetY() - start.GetY()) * t
    );
}

