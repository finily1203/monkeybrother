// LoadingScreen.cpp
#include "LoadingScreen.h"
#include "GlobalCoordinator.h"
#include "GlfwFunctions.h"
#include "GUIGameViewport.h"
#include "BackgroundComponent.h"
#include "CameraSystem2D.h"

LoadingScreen::LoadingScreen()
    : m_isLoading(false), m_loadingTimer(0.0f), m_loadingDuration(2.0f), m_targetSceneNum(0),
    m_backgroundEntity(0), m_textEntity(0), m_mossballEntity(0), m_bubblesEntity(0)
{
}

LoadingScreen::~LoadingScreen()
{
    cleanup();
}

void LoadingScreen::initialise()
{
    m_isLoading = false;
    m_loadingTimer = 0.0f;
    m_loadingDuration = 2.0f;
}

void LoadingScreen::update()
{
    if (!m_isLoading)
        return;

    m_loadingTimer += GLFWFunctions::delta_time;

    // When loading is complete, transition to target scene
    if (m_loadingTimer >= m_loadingDuration)
    {
        destroyLoadingEntities();
        GameViewWindow::setSceneNum(m_targetSceneNum);
        ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetSaveJSONPath(m_targetSceneNum));
        GLFWFunctions::newSceneLoaded = true;
        m_isLoading = false;
    }
}

void LoadingScreen::cleanup()
{
    if (m_isLoading)
    {
        destroyLoadingEntities();
        m_isLoading = false;
    }
}

SystemType LoadingScreen::getSystem()
{
    return SystemType::LoadingScreenType; // You'll need to add this to your SystemType enum
}

void LoadingScreen::startLoading(int targetSceneNum)
{
    if (m_isLoading)
        return;

    m_isLoading = true;
    m_loadingTimer = 0.0f;
    m_targetSceneNum = targetSceneNum;

    // First clean up any existing entities
    for (auto& entity : ecsCoordinator.getAllLiveEntities())
    {
        ecsCoordinator.destroyEntity(entity);
    }
	cameraSystem.setCameraPosition({ 0,0 });
    // Now create loading screen entities
    createLoadingEntities();
}

bool LoadingScreen::isLoading() const
{
    return m_isLoading;
}

void LoadingScreen::createLoadingEntities()
{
    // Reset layer manager (to avoid issues with previous scene layers)
    for (int i = 1; i < layerManager.getLayerCount(); i++)
    {
        layerManager.clearLayer(i);
    }
    layerManager.addNewLayer(); // Add layer 0

	// 0. create placeholder entity
	Entity placeholderEntity = ecsCoordinator.createEntity();
	ecsCoordinator.addComponent<TransformComponent>(placeholderEntity, TransformComponent{});
	ecsCoordinator.setEntityID(placeholderEntity, "placeholderentity");
	ecsCoordinator.setTextureID(placeholderEntity, "");

    // 1. Create background entity
    m_backgroundEntity = ecsCoordinator.createEntity();
    TransformComponent bgTransform{};
    bgTransform.position.SetX(0.0f);
    bgTransform.position.SetY(0.0f);
    bgTransform.scale.SetX(1920.0f);
    bgTransform.scale.SetY(1080.0f);
    ecsCoordinator.addComponent(m_backgroundEntity, bgTransform);

    BackgroundComponent background{};
    background.isBackground = true;
    ecsCoordinator.addComponent(m_backgroundEntity, background);

    ecsCoordinator.setEntityID(m_backgroundEntity, "loadingBackground");
    ecsCoordinator.setTextureID(m_backgroundEntity, "loadingBackground"); // Use your existing background texture

    layerManager.addEntityToLayer(0, m_backgroundEntity);

    // 2. Create loading text entity (using texture instead of font)
    m_textEntity = ecsCoordinator.createEntity();
    TransformComponent textTransform{};
    textTransform.position.SetX(0.0f);
    textTransform.position.SetY(-60.0f);
    textTransform.scale.SetX(192.0f);
    textTransform.scale.SetY(64.0f);
    ecsCoordinator.addComponent(m_textEntity, textTransform);
    
    ecsCoordinator.setEntityID(m_textEntity, "loadingText");
    ecsCoordinator.setTextureID(m_textEntity, "loadingText"); // Assuming you have a texture named "loadingText"
    
    layerManager.addEntityToLayer(0, m_textEntity);

    // 3. Create animated mossball entity
    m_mossballEntity = ecsCoordinator.createEntity();
    TransformComponent mossballTransform{};
    mossballTransform.position.SetX(50.0f);
    mossballTransform.position.SetY(0.0f);
    mossballTransform.scale.SetX(150.0f);
    mossballTransform.scale.SetY(150.0f);
    ecsCoordinator.addComponent(m_mossballEntity, mossballTransform);

    AnimationComponent mossballAnim{};
    mossballAnim.isAnimated = true;
    mossballAnim.totalFrames = 16.0f;
    mossballAnim.frameTime = 0.05f;
    mossballAnim.columns = 4.0f;
    mossballAnim.rows = 4.0f;
    ecsCoordinator.addComponent(m_mossballEntity, mossballAnim);

    ecsCoordinator.setEntityID(m_mossballEntity, "loadingMossball");
    ecsCoordinator.setTextureID(m_mossballEntity, "loadmossball");

    layerManager.addEntityToLayer(0, m_mossballEntity);

    // 4. Create animated bubbles entity
    m_bubblesEntity = ecsCoordinator.createEntity();
    TransformComponent bubblesTransform{};
    bubblesTransform.position.SetX(-50.0f);
    bubblesTransform.position.SetY(0.0f);
    bubblesTransform.scale.SetX(300.0f);
    bubblesTransform.scale.SetY(300.0f);
    ecsCoordinator.addComponent(m_bubblesEntity, bubblesTransform);

    AnimationComponent bubblesAnim{};
    bubblesAnim.isAnimated = true;
    bubblesAnim.totalFrames = 16.0f;
    bubblesAnim.frameTime = 0.05f;
    bubblesAnim.columns = 4.0f;
    bubblesAnim.rows = 4.0f;
    ecsCoordinator.addComponent(m_bubblesEntity, bubblesAnim);

    ecsCoordinator.setEntityID(m_bubblesEntity, "loadingBubbles");
    ecsCoordinator.setTextureID(m_bubblesEntity, "loadbubble");

    layerManager.addEntityToLayer(0, m_bubblesEntity);
}

void LoadingScreen::destroyLoadingEntities()
{
    // Destroy all loading screen entities
    if (m_backgroundEntity)
        ecsCoordinator.destroyEntity(m_backgroundEntity);

    if (m_textEntity)
        ecsCoordinator.destroyEntity(m_textEntity);

    if (m_mossballEntity)
        ecsCoordinator.destroyEntity(m_mossballEntity);

    if (m_bubblesEntity)
        ecsCoordinator.destroyEntity(m_bubblesEntity);

    m_backgroundEntity = 0;
    m_textEntity = 0;
    m_mossballEntity = 0;
    m_bubblesEntity = 0;
}