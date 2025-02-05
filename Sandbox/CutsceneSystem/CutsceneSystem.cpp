/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
*/

#include "CutsceneSystem.h"
#include "GlobalCoordinator.h"
#include "GlfwFunctions.h"

CutsceneSystem::CutsceneSystem() : m_currentFrameIndex(0), m_currentFrameTime(0.0f),
m_isPlaying(false), m_originalZoom(1.0f) {}

void CutsceneSystem::initialise() {
    // Reset all cutscene data to initial state
    m_frames.clear();
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;
    m_isPlaying = false;
}

void CutsceneSystem::update() {
    if (!m_isPlaying || m_currentFrameIndex >= m_frames.size()) {
        if (m_isPlaying) {
            // Cutscene just finished - restore zoom
            cameraSystem.setCameraZoom(m_originalZoom);
            m_isPlaying = false;
        }
        return;
    }

    auto& currentFrame = m_frames[m_currentFrameIndex];
    m_currentFrameTime += GLFWFunctions::delta_time;

    std::cout << "Playing frame " << m_currentFrameIndex << " time: " << m_currentFrameTime << "/" << currentFrame.duration << std::endl;
    // Update camera position directly
    cameraSystem.setCameraPosition(currentFrame.cameraPosition);

    // Move to next frame when duration is up
    if (m_currentFrameTime >= currentFrame.duration) {
        currentFrame.hasCompleted = true;
        m_currentFrameIndex++;
        m_currentFrameTime = 0.0f;

        if (m_currentFrameIndex >= m_frames.size()) {
            m_isPlaying = false;
        }
    }
}

void CutsceneSystem::cleanup() {
    m_frames.clear();
    m_isPlaying = false;
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;
}

SystemType CutsceneSystem::getSystem() {
    return SystemType::CutsceneSystemType;
}

void CutsceneSystem::addFrame(const myMath::Vector2D& position, float duration) {
    m_frames.emplace_back(position, duration);
}

void CutsceneSystem::start() {
    if (m_frames.empty()) {
        return;
    }

    m_isPlaying = true;
    m_currentFrameIndex = 0;
    m_currentFrameTime = 0.0f;

    // Set appropriate zoom level for viewing panels
    cameraSystem.setCameraZoom(0.5f); // Adjust this value as needed

    // Set initial camera position
    cameraSystem.setCameraPosition(m_frames[0].cameraPosition);
}

void CutsceneSystem::stop() {
    m_isPlaying = false;

    // Restore original zoom
    cameraSystem.setCameraZoom(m_originalZoom);
}

bool CutsceneSystem::isFinished() const {
    return !m_isPlaying && m_currentFrameIndex >= m_frames.size();
}

void CutsceneSystem::skipToEnd() {
    if (!m_frames.empty()) {
        // Move camera to final position
        cameraSystem.setCameraPosition(m_frames.back().cameraPosition);
    }

    // Mark cutscene as complete
    m_currentFrameIndex = m_frames.size();
    m_isPlaying = false;
}

myMath::Vector2D CutsceneSystem::lerp(const myMath::Vector2D& start, const myMath::Vector2D& end, float t) {
    // Clamp t between 0 and 1
    t = std::max(0.0f, std::min(1.0f, t));

    return myMath::Vector2D(
        start.GetX() + (end.GetX() - start.GetX()) * t,
        start.GetY() + (end.GetY() - start.GetY()) * t
    );
}