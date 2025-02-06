/*
All content @ 2025 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  CutsceneSystem.h
@brief  :  This file contains the declaration of the CutsceneSystem class. It is
		  responsible for managing the cutscene's camera position,
		  rotation, and zoom, as well as updating the view matrix based on these values.
		  The CutsceneSystem class also provides functionality to add frames to the cutscene,
		  start, stop, and skip to the end of the cutscene.
		  The CutsceneSystem class also provides functionality to check if the cutscene is playing or finished.

 File Contributions: Liu YaoTing (100%)

/*_______________________________________________________________________________________________________________*/

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
    static std::vector<CutsceneFrame>* m_frames;
    size_t m_currentFrameIndex;
    float m_currentFrameTime;
    bool m_isPlaying;
    float m_originalZoom;
    myMath::Vector2D lerp(const myMath::Vector2D& start, const myMath::Vector2D& end, float t);
};