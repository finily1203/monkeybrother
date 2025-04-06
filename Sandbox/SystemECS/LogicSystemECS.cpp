/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan), Ian Loi (ian.loi)
@team:   MonkeHood
@course: CSD2401
@file:   LogicSystemECS.cpp
@brief:  The source file includes all the implementation of the Logic for all entities.
		 Logic System is part of the ECS system, which handles the logic / scripts for
		 all entities in the game. Currently, it handles the jumping of player since our
		 current game demo is rotational based, there is no movement keys for the player.
		 Logic system also takes care of the enemy movement as well as camera movement.
		 Interacts with other ECS systems such as PhyColliSystemECS. Logic system now also
		 handles Behaviour logic like mouse on-click and on-hover behaviour.

		 Joel Chu (c.weiyuan): Implemented all of the functions that belongs to
							   the LogicSystemECS.
							   90%
		 Ian Loi  (ian.loi)  : Implemented functions that belongs to the MouseBehaviour
							   class which inherits from BehaviourECS.
							   10%
*//*___________________________________________________________________________-*/

#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"
#include "NavigationArrow.h"

#include "Debug.h"
#include "GUIConsole.h"
#include "GUIGameViewport.h"

void LogicSystemECS::initialise() {
	NavigationArrow::Initialize();
}

void LogicSystemECS::cleanup() {
	// Call NavigationArrow cleanup
	NavigationArrow::Cleanup();
	behaviours.clear();
}


void LogicSystemECS::update(float dt) {
	//check each layer if they visible, if layer not visible, do not update
	for (int i = 0; i < layerManager.getLayerCount(); i++) {
		bool isLayerVisible = layerManager.getLayerVisibility(i);
		if (isLayerVisible) {
			for (auto entity : layerManager.getEntitiesFromLayer(i)) {
				if (behaviours.find(entity) != behaviours.end()) {
					behaviours[entity]->update(entity);
				}
			}
		}
	}
	// Update navigation arrows
	NavigationArrow::Update();
	//NavigationArrow::Reset();
	// Check for collectables without navigation arrows
	for (auto entity : ecsCoordinator.getAllLiveEntities()) {
		if (ecsCoordinator.hasComponent<CollectableComponent>(entity)) {
			NavigationArrow::CreateNavigationArrow(entity);
		}
	}
	if (GLFWFunctions::useMouseRotation) {
		glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	(void)dt;
}

void LogicSystemECS::assignBehaviour(Entity entity, std::shared_ptr<BehaviourECS> behaviour) {
	behaviours[entity] = behaviour;
}

void LogicSystemECS::unassignBehaviour(Entity entity) {
	behaviours.erase(entity);
}


void MouseBehaviour::update(Entity entity) {
	if (glfwGetWindowAttrib(GLFWFunctions::pWindow, GLFW_HOVERED))
	{
		double mouseX{}, mouseY{};
		int windowWidth{}, windowHeight{};
		glfwGetCursorPos(GLFWFunctions::pWindow, &mouseX, &mouseY);
		glfwGetWindowSize(GLFWFunctions::pWindow, &windowWidth, &windowHeight);

		float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);
		float cursorYCentered = (windowHeight / 2.f) - static_cast<float>(mouseY);

		if (!GLFWFunctions::debug_flag)
		{
			onMouseHover(static_cast<double>(cursorXCentered), static_cast<double>(cursorYCentered));
		}
	}

	(void)entity;
}

void MouseBehaviour::setUpButtonActions()
{
	std::ifstream inputFile(FilePathManager::GetButtonsIdJSONPath());
	nlohmann::json buttonsIdJSON;

	if (inputFile.is_open())
	{
		inputFile >> buttonsIdJSON;
		inputFile.close();
	}

	std::vector<std::function<void()>> buttonFunctions = {
		[this]() { handleStartButton(); },				// start button logic in main menu		
		[this]() { handleQuitButton(pWindow); },		// quit button logic in the original file
		[this]() { handleQuitButton(pWindow); },		// quit button logic in main menu
		[this]() { handleOptionsButton(); },			// open options button logic from main menu
		[this]() { handleOptionsButton(); },			// open options button logic from pause menu
		[this]() { handleTutorialButton(); },			// open tutorial button logic from main menu
		[this]() { handleTutorialButton(); },			// open tutorial button logic from pause menu
		[this]() { handleResumeButton(); },				// resume button logic
		[this]() { handleResumeButton(); },				// close pause menu logic
		[this]() { handleCloseOptionsButton(); },		// close options button logic
		[this]() { handleCloseTutorialButton(); },		// close tutorial button logic
		[this]() { handleNextPageButton(); },			// next page button logic for tutorial menu
		[this]() { handlePreviousPageButton(); },		// previous page button logic for tutorial menu
		[this]() { handlePauseQuitButton(); },			// quit game button logic in pause menu
		[this]() { handleRetryButton(); },				// retry button logic from pause menu
		[this]() { handleRetryButton(); },				// retry button logic from game over menu
		[this]() { handleAudioBarDrag("sfxSoundbarBase"); },			// sfx bar dragging logic
		[this]() { handleAudioBarDrag("musicSoundbarBase"); },			// music bar dragging logic
		[this]() { handleConfirmButton(); },			// confirm options settings button logic
		[this]() { handleRotationSpeedSlider("rotationSpeedSlider"); },	// rotation speed dragging logic
		[this]() { handleQuitToMainMenuButton(); },		// yes button logic from confirm quit menu
		[this]() { handleQuitToMainMenuButton(); },		// main menu button logic from level completed menu
		[this]() { handleQuitToMainMenuButton(); },		// main menu button logic from game over menu
		[this]() { handleReturnToPauseMenuButton(); },	// no button logic from confirm quit menu
		[this]() { handleNextLevelButton(); },			// next level button from level completed menu
		[this]() { handleStartTutorialButton(); }		// start tutorial button from the tutorial menu
	};

	int index{};
	for (auto const& buttonId : buttonsIdJSON["buttons"])
	{
		if (index < buttonFunctions.size())
		{
			buttonActions[buttonId] = buttonFunctions[index];
		}

		index++;
	}
}

// function that handles the logic for start button in the main menu
void MouseBehaviour::handleStartButton()
{
	// resetting the values back to the original values and setting the scene variable to 1
	GLFWFunctions::gamePaused = false;
	GLFWFunctions::optionsMenuCount = 0;
	auto allEntities = ecsCoordinator.getAllLiveEntities();

	cameraSystem.setCameraZoom(0.8f);
	for (auto currEntity : allEntities)
	{
		ecsCoordinator.destroyEntity(currEntity);
	}

	audioSystem.playSoundEffect("UI_ButtonClick.wav");
	//set scene to play cutscene
	GameViewWindow::setSceneNum(-2); // Cutscene scene number
	ecsCoordinator.LoadIntroCutsceneFromJSON(ecsCoordinator, FilePathManager::GetIntroCutsceneJSONPath());
}

// function that handles the logic for quit game button 
void MouseBehaviour::handleQuitButton(GLFWwindow* window)
{
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// function that handles the logic for the options menu button
void MouseBehaviour::handleOptionsButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	// if pause menu already exists, destroy the pause menu
	if (GLFWFunctions::pauseMenuCount == 1)
	{
		for (auto currEntity : allEntities)
		{
			if (ecsCoordinator.getEntityID(currEntity) == "pauseMenuBg" ||
				ecsCoordinator.getEntityID(currEntity) == "closePauseMenu" ||
				ecsCoordinator.getEntityID(currEntity) == "resumeButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseOptionsButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseTutorialButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseQuitButton")
			{
				ecsCoordinator.destroyEntity(currEntity);
			}
		}

		// decrement the count since the pause menu is already destroyed
		GLFWFunctions::pauseMenuCount--;
	}

	// ensure that there an options menu does not exist in the scene before creating and loading
	// the options menu to the scene
	if (GLFWFunctions::optionsMenuCount < 1)
	{
		GLFWFunctions::optionsMenuCount++;
		ecsCoordinator.LoadOptionsMenuFromJSON(ecsCoordinator, FilePathManager::GetOptionsMenuJSONPath());

		Entity sfxArrowEntity = ecsCoordinator.getEntityFromID("sfxSoundbarArrow");
		if (ecsCoordinator.hasComponent<TransformComponent>(sfxArrowEntity))
		{
			TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(sfxArrowEntity);
			transform.position.SetX(GLFWFunctions::sfxArrowPos);
		}

		// Update music arrow position
		Entity musicArrowEntity = ecsCoordinator.getEntityFromID("musicSoundbarArrow");
		if (ecsCoordinator.hasComponent<TransformComponent>(musicArrowEntity))
		{
			TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(musicArrowEntity);
			transform.position.SetX(GLFWFunctions::musicArrowPos);
		}

		// Update rotation speed slider notch position
		Entity rotationSliderEntity = ecsCoordinator.getEntityFromID("rotationSpeedSliderNotch");
		if (ecsCoordinator.hasComponent<TransformComponent>(rotationSliderEntity))
		{
			TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(rotationSliderEntity);
			transform.position.SetX(GLFWFunctions::rotationSpeedPos);
		}
	}
}

// function that handles the logic for the tutorial button
void MouseBehaviour::handleTutorialButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	if (GLFWFunctions::pauseMenuCount == 1)
	{
		for (auto currEntity : allEntities)
		{
			if (ecsCoordinator.getEntityID(currEntity) == "pauseMenuBg" ||
				ecsCoordinator.getEntityID(currEntity) == "closePauseMenu" ||
				ecsCoordinator.getEntityID(currEntity) == "resumeButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseOptionsButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseTutorialButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseQuitButton" ||
				ecsCoordinator.getEntityID(currEntity) == "tutorialClick")
			{
				ecsCoordinator.destroyEntity(currEntity);
			}
		}

		// decrement the count since the pause menu is already destroyed
		GLFWFunctions::pauseMenuCount--;
	}

	if (GLFWFunctions::tutorialMenuCount < 1)
	{
		GLFWFunctions::tutorialMenuCount++;
		ecsCoordinator.LoadTutorialMenuFromJSON(ecsCoordinator, FilePathManager::GetTutorialJSONPath());
	}
}

// function that handles the logic for the resume button and close pause menu button
void MouseBehaviour::handleResumeButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	// destroy the pause menu 
	for (auto currEntity : allEntities)
	{
		if (ecsCoordinator.getEntityID(currEntity) == "pauseMenuBg" ||
			ecsCoordinator.getEntityID(currEntity) == "closePauseMenu" ||
			ecsCoordinator.getEntityID(currEntity) == "resumeButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseOptionsButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseTutorialButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseQuitButton")
		{
			ecsCoordinator.destroyEntity(currEntity);
		}
	}

	// set the game pause state to be false and decrement the pause menu count
	GLFWFunctions::gamePaused = false;
	GLFWFunctions::pauseMenuCount--;
}

// function that handles the logic for close options menu
void MouseBehaviour::handleCloseOptionsButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	std::unordered_set<std::string> optionsMenuEntityNames = {
		"optionsMenuBg", "closeOptionsMenu", "confirmButton",
		"sfxSoundbarBase", "musicSoundbarBase", "sfxSoundbarArrow",
		"musicSoundbarArrow", "sfxNotch0", "sfxNotch1", "sfxNotch2",
		"sfxNotch3", "sfxNotch4", "sfxNotch5", "sfxNotch6", "sfxNotch7",
		"sfxNotch8", "sfxNotch9", "musicNotch0", "musicNotch1", "musicNotch2",
		"musicNotch3", "musicNotch4", "musicNotch5", "musicNotch6", "musicNotch7",
		"musicNotch8", "musicNotch9", "rotationSpeedSlider", "rotationSpeedSliderNotch"
	};

	// destroy the options menu
	for (auto currEntity : allEntities)
	{
		if (optionsMenuEntityNames.count(ecsCoordinator.getEntityID(currEntity)))
		{
			ecsCoordinator.destroyEntity(currEntity);
		}
	}

	// decrement the options menu count
	GLFWFunctions::optionsMenuCount--;

	// checks the current scene is a game level, not the main menu scene and a pause menu does 
	// not exist in the current scene
	if (GameViewWindow::getSceneNum() > -1 && GLFWFunctions::pauseMenuCount < 1)
	{
		// load the pause menu and increment the pause menu count
		ecsCoordinator.LoadPauseMenuFromJSON(ecsCoordinator, FilePathManager::GetPauseMenuJSONPath());
		GLFWFunctions::pauseMenuCount++;
	}

	// set the game pause state to true
	GLFWFunctions::gamePaused = true;
	cameraSystem.readGameplaySettingsFromJSON(FilePathManager::GetGameplaySettingsJSONPath());
}

// function that handles the logic for closing of tutorial menu button
void MouseBehaviour::handleCloseTutorialButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	for (auto currEntity : allEntities)
	{
		if (ecsCoordinator.getEntityID(currEntity) == "tutorialBaseBg" ||
			ecsCoordinator.getEntityID(currEntity) == "closeTutorialMenu" ||
			ecsCoordinator.getEntityID(currEntity) == "pageCounter" ||
			ecsCoordinator.getEntityID(currEntity) == "nextTutorialPage" ||
			ecsCoordinator.getEntityID(currEntity) == "previousTutorialPage" ||
			ecsCoordinator.getEntityID(currEntity) == "tutorialClick")
		{
			ecsCoordinator.destroyEntity(currEntity);
		}
	}

	GLFWFunctions::tutorialMenuCount--;
	GLFWFunctions::tutorialCurrentPage = 1;

	// checks the current scene is a game level, not the main menu scene and a pause menu does 
	// not exist in the current scene
	if (GameViewWindow::getSceneNum() > -1 && GLFWFunctions::pauseMenuCount < 1)
	{
		// load the pause menu and increment the pause menu count
		ecsCoordinator.LoadPauseMenuFromJSON(ecsCoordinator, FilePathManager::GetPauseMenuJSONPath());
		GLFWFunctions::pauseMenuCount++;
	}

	// set the game pause state to true
	GLFWFunctions::gamePaused = true;
}

// button that handles the logic for next page button for the tutorial menu
void MouseBehaviour::handleNextPageButton()
{
	GLFWFunctions::tutorialCurrentPage++;
}

// function that handles the logic for previous page button for the tutorial menu
void MouseBehaviour::handlePreviousPageButton()
{
	GLFWFunctions::tutorialCurrentPage--;
}

// function that handles the logic for quit game button inside the pause menu
void MouseBehaviour::handlePauseQuitButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	// destroy all the entities in the current scene
	for (auto currEntity : allEntities)
	{
		if (ecsCoordinator.getEntityID(currEntity) == "pauseMenuBg" ||
			ecsCoordinator.getEntityID(currEntity) == "closePauseMenu" ||
			ecsCoordinator.getEntityID(currEntity) == "resumeButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseOptionsButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseTutorialButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseRetryButton" ||
			ecsCoordinator.getEntityID(currEntity) == "pauseQuitButton")
		{
			ecsCoordinator.destroyEntity(currEntity);
		}
	}

	// decrement the pause menu count and load the main menu back into the scene
	GLFWFunctions::pauseMenuCount--;
	GLFWFunctions::quitLevelMenuCount++;
	ecsCoordinator.LoadQuitLevelMenuFromJSON(ecsCoordinator, FilePathManager::GetQuitLevelMenuJSONPath());
}

// function that handles the logic for retry level button
void MouseBehaviour::handleRetryButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();

	// destroying all entities in the scene
	for (auto currEntity : allEntities)
	{
		ecsCoordinator.destroyEntity(currEntity);
	}

	audioSystem.playSoundEffect("UI_ButtonClick.wav");
	// resetting all values
	GLFWFunctions::gameOver = false;
	GLFWFunctions::gamePaused = false;
	GLFWFunctions::pauseMenuCount = 0;
	GLFWFunctions::optionsMenuCount = 0;
	GLFWFunctions::gameOverMenuCount = 0;
	GLFWFunctions::newSceneLoaded = true;

	// reloading the scene based on the scene number
	if (GameViewWindow::getSceneNum() != 0)
	{
		int scene = GameViewWindow::getSceneNum();
		ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetSaveJSONPath(scene));
	}

	else
	{
		ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetEntitiesJSONPath());
	}
}

// function that handles the logic for both sfx and music audio bar slider dragging
void MouseBehaviour::handleAudioBarDrag(std::string const& entityId)
{
	// getting the window's width, height and cursor position x and y values
	double mouseX{}, mouseY{};
	int windowWidth{}, windowHeight{};
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	glfwGetCursorPos(GLFWFunctions::pWindow, &mouseX, &mouseY);
	glfwGetWindowSize(GLFWFunctions::pWindow, &windowWidth, &windowHeight);
	setSoundbarId(entityId);

	// finding the actual mouse cursor position based on the window dimensions
	float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);
	// determine which audio arrow will be used
	std::string audioArrowId = (entityId == "sfxSoundbarBase") ? "sfxSoundbarArrow" : "musicSoundbarArrow";

	// looping through all live entities
	for (auto& currEntity : allEntities)
	{
		// update the arrow position based on the arrow entity Id
		if (ecsCoordinator.getEntityID(currEntity) == audioArrowId)
		{
			TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(currEntity);
			transform.position.SetX(cursorXCentered);
			break;
		}
	}
}

// function that handles the logic for rotation speed slider 
void MouseBehaviour::handleRotationSpeedSlider(std::string const& entityId)
{
	// getting the window's width, height and cursor position x and y values
	double mouseX{}, mouseY{};
	int windowWidth{}, windowHeight{};
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	glfwGetCursorPos(GLFWFunctions::pWindow, &mouseX, &mouseY);
	glfwGetWindowSize(GLFWFunctions::pWindow, &windowWidth, &windowHeight);
	setSliderId(entityId);
	std::string const& currentSlider = getSliderId();

	// finding the actual mouse cursor position based on the window dimensions
	float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);

	std::string sliderNotchId = (entityId == "rotationSpeedSlider") ? "rotationSpeedSliderNotch" : "";

	TransformComponent sliderTransform{}, notchTransform{};
	bool foundSlider = false;
	bool foundSliderNotch = false;

	for (auto& currEntity : allEntities)
	{
		if (ecsCoordinator.getEntityID(currEntity) == entityId)
		{
			sliderTransform = ecsCoordinator.getComponent<TransformComponent>(currEntity);
			foundSlider = true;
		}

		else if (ecsCoordinator.getEntityID(currEntity) == sliderNotchId)
		{
			notchTransform = ecsCoordinator.getComponent<TransformComponent>(currEntity);
			foundSliderNotch = true;
		}

		if (foundSlider && foundSliderNotch)
		{
			break;
		}
	}

	float notchHalfWidth = notchTransform.scale.GetX() / 2.f;
	float sliderLeft = sliderTransform.position.GetX() - (sliderTransform.scale.GetX() / 2.f) + notchHalfWidth;
	float sliderRight = sliderTransform.position.GetX() + (sliderTransform.scale.GetX() / 1.95f) - notchHalfWidth;


	for (auto& currEntity : allEntities)
	{
		if (ecsCoordinator.getEntityID(currEntity) == sliderNotchId)
		{
			TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(currEntity);
			transform.position.SetX(cursorXCentered);

			if (currentSlider == "rotationSpeedSlider")
			{
				float normalizedPos = (cursorXCentered - sliderLeft) / (sliderRight - sliderLeft);
				GLFWFunctions::rotationSpeed = static_cast<int>(90.f + (normalizedPos * 64.f));
				GLFWFunctions::rotationSpeed = static_cast<int>(std::ceil((GLFWFunctions::rotationSpeed / 10)) * 10);
				GLFWFunctions::rotationSpeed = std::max(90, std::min(150, GLFWFunctions::rotationSpeed));
			}

			break;
		}
	}
}

// function that handles the logic for confirm button for the options settings
void MouseBehaviour::handleConfirmButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	std::unordered_set<std::string> optionsMenuEntityNames = {
		"optionsMenuBg", "closeOptionsMenu", "confirmButton",
		"sfxSoundbarBase", "musicSoundbarBase", "sfxSoundbarArrow",
		"musicSoundbarArrow", "sfxNotch0", "sfxNotch1", "sfxNotch2",
		"sfxNotch3", "sfxNotch4", "sfxNotch5", "sfxNotch6", "sfxNotch7",
		"sfxNotch8", "sfxNotch9", "musicNotch0", "musicNotch1", "musicNotch2",
		"musicNotch3", "musicNotch4", "musicNotch5", "musicNotch6", "musicNotch7",
		"musicNotch8", "musicNotch9", "rotationSpeedSlider", "rotationSpeedSliderNotch"
	};

	// initializing sfxPercentage and musicPercentage variables
	float sfxPercentage = AudioSystem::sfxPercentage;
	float musicPercentage = AudioSystem::musicPercentage;

	int rotationSpeed = GLFWFunctions::rotationSpeed;

	// save the new audio arrow (for both sfx and music) position x to the options menu JSON file
	//ecsCoordinator.SaveOptionsSettingsToJSON(ecsCoordinator, FilePathManager::GetOptionsMenuJSONPath());
	// save the sfx and music percentages to the audio settings JSON file
	//audioSystem.saveAudioSettingsToJSON(FilePathManager::GetAudioSettingsJSONPath(), sfxPercentage, musicPercentage);
	//cameraSystem.saveGameplaySettingsToJSON(FilePathManager::GetGameplaySettingsJSONPath(), rotationSpeed);

	//change on audio side as well
	audioSystem.setGenVol(musicPercentage);
	audioSystem.setBgmVol(musicPercentage);
	audioSystem.setSfxVol(sfxPercentage);

	Entity sfxArrowEntity = ecsCoordinator.getEntityFromID("sfxSoundbarArrow");
	if (ecsCoordinator.hasComponent<TransformComponent>(sfxArrowEntity)) 
	{
		TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(sfxArrowEntity);
		transform.position.SetX(GLFWFunctions::sfxArrowPos);
	}

	// Update music arrow position
	Entity musicArrowEntity = ecsCoordinator.getEntityFromID("musicSoundbarArrow");
	if (ecsCoordinator.hasComponent<TransformComponent>(musicArrowEntity)) 
	{
		TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(musicArrowEntity);
		transform.position.SetX(GLFWFunctions::musicArrowPos);
	}

	// Update rotation speed slider notch position
	Entity rotationSliderEntity = ecsCoordinator.getEntityFromID("rotationSpeedSliderNotch");
	if (ecsCoordinator.hasComponent<TransformComponent>(rotationSliderEntity)) 
	{
		TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(rotationSliderEntity);
		transform.position.SetX(GLFWFunctions::rotationSpeedPos);
	}

	for (auto currEntity : allEntities)
	{
		if (optionsMenuEntityNames.count(ecsCoordinator.getEntityID(currEntity)))
		{
			ecsCoordinator.destroyEntity(currEntity);
		}
	}

	// decrement the options menu count
	GLFWFunctions::optionsMenuCount--;

	// check that the current scene is a level scene and a pause menu does not exist in the
	// current scene
	if (GameViewWindow::getSceneNum() > -1 && GLFWFunctions::pauseMenuCount < 1)
	{
		// load the pause menu and increment the pause menu count
		ecsCoordinator.LoadPauseMenuFromJSON(ecsCoordinator, FilePathManager::GetPauseMenuJSONPath());
		GLFWFunctions::pauseMenuCount++;
	}

	// set the game pause state to true
	GLFWFunctions::gamePaused = true;
}

// function that handles the logic for quit game button and main menu button
void MouseBehaviour::handleQuitToMainMenuButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");
	GLFWFunctions::gamePaused = false;
	int mainMenuScene = -1;

	if (GLFWFunctions::levelCompletedMenuCount == 1)
	{
		GLFWFunctions::levelCompletedMenuCount--;

		if (GameViewWindow::getSceneNum() > 4)
		{
			//if is tutorial scene go to main menu not cutscene
			if (GameViewWindow::getSceneNum() != 11 && GameViewWindow::getSceneNum() != 12 && GameViewWindow::getSceneNum() != 13)
			{
				mainMenuScene = -4;
			}
		}
	}

	else if (GLFWFunctions::gameOverMenuCount == 1)
	{
		GLFWFunctions::gameOverMenuCount--;
	}

	else if (GLFWFunctions::quitLevelMenuCount == 1)
	{
		GLFWFunctions::quitLevelMenuCount--;
	}


	for (auto& currEntity : allEntities)
	{
		ecsCoordinator.destroyEntity(currEntity);
	}

	GameViewWindow::setSceneNum(mainMenuScene);
	if (mainMenuScene == -1)
		ecsCoordinator.LoadMainMenuFromJSON(ecsCoordinator, FilePathManager::GetMainMenuJSONPath());
	else
		ecsCoordinator.LoadEndCutsceneFromJSON(ecsCoordinator, FilePathManager::GetEndCutsceneJSONPath());

}

// function that handles the logic for no button that returns back to the pause menu
void MouseBehaviour::handleReturnToPauseMenuButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	if (GLFWFunctions::quitLevelMenuCount == 1)
	{
		GLFWFunctions::quitLevelMenuCount--;
	}

	for (auto& currEntity : allEntities)
	{
		if (ecsCoordinator.getEntityID(currEntity) == "quitLevelMenuBase" ||
			ecsCoordinator.getEntityID(currEntity) == "quitToMainMenuButton" ||
			ecsCoordinator.getEntityID(currEntity) == "returnToPauseMenuButton")
		{
			ecsCoordinator.destroyEntity(currEntity);
		}
	}

	GLFWFunctions::pauseMenuCount++;
	ecsCoordinator.LoadPauseMenuFromJSON(ecsCoordinator, FilePathManager::GetPauseMenuJSONPath());
}

// function that handles the logic for the next level button
void MouseBehaviour::handleNextLevelButton()
{
	GLFWFunctions::levelCompletedMenuCount--;
	audioSystem.playSoundEffect("UI_ButtonClick.wav");

	if (!GLFWFunctions::changeLevel)
	{
		int currScene = GameViewWindow::getSceneNum();
		currScene++;

		if (currScene > 5)
		{
			currScene = -1;
		}

		GameViewWindow::setSceneNum(currScene);
		GLFWFunctions::changeLevel = true;
		GLFWFunctions::newSceneLoaded = true;
	}
}

// function that handles the logic for start tutorial button
void MouseBehaviour::handleStartTutorialButton()
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	audioSystem.playSoundEffect("UnderWater_Button_press_2.wav");
	int tutorialPage = GLFWFunctions::tutorialCurrentPage; // takes note for tutorial page to select which level

	for (auto currEntity : allEntities)
	{
		if (ecsCoordinator.getEntityID(currEntity) == "tutorialBaseBg" ||
			ecsCoordinator.getEntityID(currEntity) == "closeTutorialMenu" ||
			ecsCoordinator.getEntityID(currEntity) == "pageCounter" ||
			ecsCoordinator.getEntityID(currEntity) == "nextTutorialPage" ||
			ecsCoordinator.getEntityID(currEntity) == "previousTutorialPage" ||
			ecsCoordinator.getEntityID(currEntity) == "tutorialClick") //added tutorial button
		{
			ecsCoordinator.destroyEntity(currEntity);
		}
	}

	GLFWFunctions::tutorialMenuCount--;
	GLFWFunctions::tutorialCurrentPage = 1;

	// checks tutorial page, 
	// if page is 2, load scene Tutorial Move // scene 11
	// if page is 3 or 5, load scene Tutorial Pump // scene 13
	// if page is 7, load scene Tutorial Filter // scene 12

	GLFWFunctions::changeLevel = true;
	GLFWFunctions::newSceneLoaded = true;

	if (tutorialPage == 2)
	{
		int sceneNum = 11;
		loadingScreen.startLoading(sceneNum);
	}
	else if (tutorialPage == 3 || tutorialPage == 5)
	{
		int sceneNum = 13;
		loadingScreen.startLoading(sceneNum);
	}
	else if (tutorialPage == 7)
	{
		int sceneNum = 12;
		loadingScreen.startLoading(sceneNum);
	}
}


// function that handles logic code for mouse click action, mainly mouse click for buttons
void MouseBehaviour::onMouseClick(GLFWwindow* window, double mouseX, double mouseY)
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	int optionsMenuLayer = layerManager.getEntityLayer(ecsCoordinator.getEntityFromID("optionsMenuBg"));
	int tutorialMenuLayer = layerManager.getEntityLayer(ecsCoordinator.getEntityFromID("tutorialBaseBg"));

	// looping through all the live entities in the scene
	for (auto& entity : allEntities)
	{
		// ensuring that the entity is a button if it contains a button component
		if (ecsCoordinator.hasComponent<ButtonComponent>(entity))
		{
			int entityLayer = layerManager.getEntityLayer(entity);

			if (GLFWFunctions::optionsMenuCount > 0 || GLFWFunctions::tutorialMenuCount > 0)
			{
				if (entityLayer < optionsMenuLayer || entityLayer < tutorialMenuLayer)
				{
					continue;
				}
			}

			//check if entity is visible
			if (layerManager.getEntityVisibility(entity))
			{
				TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

				// this checks that the mouse cursor is over the button
				if (mouseIsOverButton(mouseX, mouseY, transform))
				{
					// this handles the logic for the particular button click
					handleButtonClick(window, entity);

					std::string entityId = ecsCoordinator.getEntityID(entity);

					// this statement is only applicable when we press and hold down and drag
					// the mouse cursor on the sfxSoundbarBase and musicSoundbarBase
					if (entityId == "sfxSoundbarBase" || entityId == "musicSoundbarBase" ||
						entityId == "rotationSpeedSlider")
					{
						// set the bool isDragging to true
						isDragging = true;
					}
				}
			}
		}
	}
}

// function that handles logic for the dragging of the mouse cursor
void MouseBehaviour::onMouseDrag(GLFWwindow* window, double mouseX, double mouseY)
{
	// ensure that bool isDragging is true
	if (!isDragging)
	{
		return;
	}

	auto allEntities = ecsCoordinator.getAllLiveEntities();

	// logic to retrieve the cursor position based on the current window screen
	int windowWidth{}, windowHeight{};
	glfwGetWindowSize(GLFWFunctions::pWindow, &windowWidth, &windowHeight);
	float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);

	// setting the correct audio arrow based on the soundbar Id
	std::string soundbarArrow = (getSoundbarId() == "sfxSoundbarBase") ? "sfxSoundbarArrow" :
		(getSoundbarId() == "musicSoundbarBase") ? "musicSoundbarArrow" : "";

	std::string sliderNotch = (getSliderId() == "rotationSpeedSlider") ? "rotationSpeedSliderNotch" : "";

	// ensuring that the string is not empty
	if (!soundbarArrow.empty())
	{
		TransformComponent soundbarTransform{};

		// looping through all live entities in the scene
		for (auto& entity : allEntities)
		{
			// below is the logic for finding the actual soundbar based on the soundbar Id that 
			// you are dragging on
			std::string entityId = ecsCoordinator.getEntityID(entity);
			if (entityId == getSoundbarId())
			{
				soundbarTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
				break;
			}
		}

		// finding the minimum left boundary and maximum right boundary of the soundbar
		float soundbarLeft = soundbarTransform.position.GetX() - (soundbarTransform.scale.GetX() / 2.12f);
		float soundbarRight = soundbarTransform.position.GetX() + (soundbarTransform.scale.GetX() / 2.1f);

		// ensure that your cursor is within the range of both the sfx and musicc soundbarBase
		if (cursorXCentered >= soundbarLeft && cursorXCentered <= soundbarRight)
		{
			// below is the logic that updates the audio arrow position based on the mouse cursor
			// position when dragging
			for (auto& entity : allEntities)
			{
				std::string entityId = ecsCoordinator.getEntityID(entity);

				if (entityId == soundbarArrow)
				{
					TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					transform.position.SetX(cursorXCentered);
					break;
				}
			}
		}
	}

	else if (!sliderNotch.empty())
	{
		TransformComponent sliderTransform{}, notchTransform{};
		bool foundSlider = false;
		bool foundSliderNotch = false;

		for (auto& entity : allEntities)
		{
			std::string entityId = ecsCoordinator.getEntityID(entity);
			if (entityId == getSliderId())
			{
				sliderTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
				foundSlider = true;
			}

			else if (entityId == sliderNotch)
			{
				notchTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
				foundSliderNotch = true;
			}

			if (foundSlider && foundSliderNotch)
			{
				break;
			}
		}

		float notchHalfWidth = notchTransform.scale.GetX() / 2.f;
		float sliderLeft = sliderTransform.position.GetX() - (sliderTransform.scale.GetX() / 2.f) + notchHalfWidth;
		float sliderRight = sliderTransform.position.GetX() + (sliderTransform.scale.GetX() / 2.f) - notchHalfWidth;

		if (cursorXCentered >= sliderLeft && cursorXCentered <= sliderRight)
		{
			for (auto& entity : allEntities)
			{
				std::string entityId = ecsCoordinator.getEntityID(entity);

				if (entityId == sliderNotch)
				{
					TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					transform.position.SetX(cursorXCentered);

					if (getSliderId() == "rotationSpeedSlider")
					{
						float normalizedPos = (cursorXCentered - sliderLeft) / (sliderRight - sliderLeft);
						GLFWFunctions::rotationSpeed = static_cast<int>(90.f + (normalizedPos * 64.f));
						GLFWFunctions::rotationSpeed = static_cast<int>(std::ceil((GLFWFunctions::rotationSpeed / 10)) * 10);
						GLFWFunctions::rotationSpeed = std::max(90, std::min(150, GLFWFunctions::rotationSpeed));
					}

					break;
				}
			}
		}
	}

	TransformComponent& sfxArrowTransform = ecsCoordinator.getComponent<TransformComponent>(ecsCoordinator.getEntityFromID("sfxSoundbarArrow"));
	TransformComponent& musicArrowTransform = ecsCoordinator.getComponent<TransformComponent>(ecsCoordinator.getEntityFromID("musicSoundbarArrow"));
	TransformComponent& rotationSpeedTransform = ecsCoordinator.getComponent<TransformComponent>(ecsCoordinator.getEntityFromID("rotationSpeedSliderNotch"));

	GLFWFunctions::sfxArrowPos = sfxArrowTransform.position.GetX();
	GLFWFunctions::musicArrowPos = musicArrowTransform.position.GetX();
	GLFWFunctions::rotationSpeedPos = rotationSpeedTransform.position.GetX();

	(void)window;
	(void)mouseY;
}

// this function handles the logic for mouse cursor on hovering
void MouseBehaviour::onMouseHover(double mouseX, double mouseY)
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	GLFWFunctions::isHovering = false;
	setHoveredButton("");

	int optionsMenuLayer = layerManager.getEntityLayer(ecsCoordinator.getEntityFromID("optionsMenuBg"));
	int tutorialMenuLayer = layerManager.getEntityLayer(ecsCoordinator.getEntityFromID("tutorialBaseBg"));

	// looping through all live entities in the current scene
	for (auto& entity : allEntities)
	{
		// checking that the entity is a button
		if (ecsCoordinator.hasComponent<ButtonComponent>(entity))
		{
			int entityLayer = layerManager.getEntityLayer(entity);

			if (GLFWFunctions::optionsMenuCount > 0 || GLFWFunctions::tutorialMenuCount > 0)
			{
				if (entityLayer < optionsMenuLayer || entityLayer < tutorialMenuLayer)
				{
					continue;
				}
			}

			//check if entity is visible
			if (layerManager.getEntityVisibility(entity))
			{
				TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

				// this checks if the mouse cursor is over the current button or not, if it is
				// over the button change the mouse cursor as well as setting the bool isHovering
				// to be true and setting the currently hovered button
				if (mouseIsOverButton(mouseX, mouseY, transform))
				{
					glfwSetCursor(GLFWFunctions::pWindow, cursor);
					GLFWFunctions::isHovering = true;
					setHoveredButton(ecsCoordinator.getEntityID(entity));
					if (!GLFWFunctions::buttonHoverAudio){
						audioSystem.playSoundEffect("MenuHoverOverSFX_2.wav");
						GLFWFunctions::buttonHoverAudio = true;
					}
					return;
				}
			}
		}
	}

	// when the mouse cursor is not hovering on top of any buttons set the cursor back to the
	// normal cursor
	if (!GLFWFunctions::isHovering)
	{
		glfwSetCursor(GLFWFunctions::pWindow, nullptr);
		GLFWFunctions::buttonHoverAudio = false;
	}
}

// this function handles the logic to check whether the mouse cursor is within the boundaries of
// the button entity
bool MouseBehaviour::mouseIsOverButton(double mouseX, double mouseY, TransformComponent& transform)
{
	float const scalar = 0.85f;
	float buttonLeft = transform.position.GetX() - transform.scale.GetX() * scalar / 1.8f;
	float buttonRight = transform.position.GetX() + transform.scale.GetX() * scalar / 1.85f;
	float buttonTop = transform.position.GetY() + transform.scale.GetY() * scalar / 2.f;
	float buttonBottom = transform.position.GetY() - transform.scale.GetY() * scalar / 2.f;

	return (mouseX >= static_cast<double>(buttonLeft) && mouseX <= static_cast<double>(buttonRight) && mouseY >= static_cast<double>(buttonBottom) && mouseY <= static_cast<double>(buttonTop));
}

// this function handles the logic for clicking of buttons
void MouseBehaviour::handleButtonClick(GLFWwindow* window, Entity entity)
{
	std::string entityId = ecsCoordinator.getEntityID(entity);
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	setSoundbarId("");
	setSliderId("");

	if (buttonActions.count(entityId))
	{
		buttonActions[entityId]();
	}

	(void)window;
}

// MouseBehaviour object instance destructor
MouseBehaviour::~MouseBehaviour()
{
	// if a cursor pointer still exists, destroy the cursor and set the cursor back to a
	// null pointer
	if (cursor)
	{
		glfwDestroyCursor(cursor);
		cursor = nullptr;
	}
}

void LogicSystemECS::ApplyForce(Entity entity, const myMath::Vector2D& appliedForce) {
	myMath::Vector2D& accForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).accumulatedForce;

	accForce.SetX(accForce.GetX() + appliedForce.GetX());
	accForce.SetY(accForce.GetY() + appliedForce.GetY());

}

std::string LogicSystemECS::getSystemECS() {
	return "LogicSystemECS";
}
