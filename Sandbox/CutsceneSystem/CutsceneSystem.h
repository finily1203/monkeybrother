/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
*/

#pragma once
#include "Systems.h"
#include "vector2D.h"
//#include "GlobalCoordinator.h"
#include <vector>


class CutsceneSystem : public GameSystems {
private:
    struct CutsceneFrame {
        myMath::Vector2D cameraPosition;
        float duration;
        bool hasCompleted;

        CutsceneFrame(const myMath::Vector2D& pos, float dur)
            : cameraPosition(pos)
            , duration(dur)
            , hasCompleted(false) {}
    };

public:
    CutsceneSystem();
    ~CutsceneSystem() override = default;

    // Required system overrides
    void initialise() override;
    void update() override;
    void cleanup() override;
    SystemType getSystem() override;

    // Cutscene-specific methods
    void addFrame(const myMath::Vector2D& position, float duration);
    void start();
    void stop();
    bool isPlaying() const { return m_isPlaying; }
    bool isFinished() const;
    void skipToEnd();

private:
    std::vector<CutsceneFrame> m_frames;
    size_t m_currentFrameIndex;
    float m_currentFrameTime;
    bool m_isPlaying;
    float m_originalZoom;
    myMath::Vector2D lerp(const myMath::Vector2D& start, const myMath::Vector2D& end, float t);
};