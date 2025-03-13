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