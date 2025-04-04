/*
All content @ 2025 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  LoadingScreen.h
@brief  :  This file contains the declaration of the loading screen. It acts as a
           transition screen between scenes, displaying a loading animation while the
           next scene is being loaded in the background. 

            File Contributions: Liu YaoTing (100%)

/*_______________________________________________________________________________________________________________*/

// LoadingScreen.h
#pragma once
#include "EngineDefinitions.h"
#include <GL/glew.h>
#include "Systems.h"
#include "ECSCoordinator.h"
#include "GraphicsSystem.h"

class LoadingScreen : public GameSystems
{
public:
    LoadingScreen();
    ~LoadingScreen();

    void initialise() override;
    void update() override;
    void cleanup() override;
    SystemType getSystem() override;

    void startLoading(int targetSceneNum);
    bool isLoading() const;

private:
    bool m_isLoading;
    float m_loadingTimer;
    float m_loadingDuration;
    int m_targetSceneNum;
    Entity m_backgroundEntity;
    Entity m_textEntity;
    Entity m_mossballEntity;
    Entity m_bubblesEntity;

    void createLoadingEntities();
    void destroyLoadingEntities();
};