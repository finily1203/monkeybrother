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

#include "Debug.h"
#include "GUIConsole.h"
#include "GUIGameViewport.h"

void LogicSystemECS::initialise() {}

void LogicSystemECS::cleanup() {
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
	//for each entity, update the behaviour
	//for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
	//	if (behaviours.find(entity) != behaviours.end()) {
	//		behaviours[entity]->update(entity);
	//	}
	//}
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
					if (entityId == "sfxSoundbarBase" || entityId == "musicSoundbarBase")
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

	// below are all the if statements that check which button is the current entity that you are
	// clicking on
	// this handles the main menu quit button
	if (entityId == "quitButton" || entityId == "quitWindowButton")
	{
		audioSystem.playSoundEffect("UI_ButtonClick.wav");

		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// this handles the retry button
	//else if (entityId == "retryButton")
	//{
	//	// destroying all entities in the scene
	//	for (auto currEntity : allEntities)
	//	{
	//		ecsCoordinator.destroyEntity(currEntity);
	//	}

	//	audioSystem.playSoundEffect("UI_ButtonClick.wav");
	//	// resetting all values
	//	GLFWFunctions::gameOver = false;
	//	GLFWFunctions::gamePaused = false;
	//	GLFWFunctions::pauseMenuCount = 0;
	//	GLFWFunctions::optionsMenuCount = 0;

	//	//ecsCoordinator.test5();

	//	// reloading the scene based on the scene number
	//	if (GameViewWindow::getSceneNum() != 0)
	//	{
	//		int scene = GameViewWindow::getSceneNum();
	//		ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetSaveJSONPath(scene));
	//	}

	//	else
	//	{
	//		ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetEntitiesJSONPath());
	//	}
	//}

	else if (entityId == "pauseRetryButton")
	{
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
		GLFWFunctions::newSceneLoaded = true;

		//ecsCoordinator.test5();

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

	// this handles the start button
	else if (entityId == "startButton")
	{
		// resetting the values back to the original values and setting the scene variable to 1
		GLFWFunctions::gamePaused = false;
		GLFWFunctions::optionsMenuCount = 0;
		//int levelOneScene = 1;

		cameraSystem.setCameraZoom(0.8f);
		for (auto currEntity : allEntities)
		{
			ecsCoordinator.destroyEntity(currEntity);
		}

		audioSystem.playSoundEffect("UI_ButtonClick.wav");
		//set scene to play cutscene
		/*ecsCoordinator.LoadIntroCutsceneFromJSON(ecsCoordinator, FilePathManager::GetIntroCutsceneJSONPath());*/
		GameViewWindow::setSceneNum(-2); // Cutscene scene number
		ecsCoordinator.LoadIntroCutsceneFromJSON(ecsCoordinator, FilePathManager::GetIntroCutsceneJSONPath());

		//GameViewWindow::setSceneNum(levelOneScene);
		//audioSystem.setChangeBGM(false);
		//GameViewWindow::SaveSceneToJSON(FilePathManager::GetSceneJSONPath());

		//if (GameViewWindow::getSceneNum() != 0)
		//{
		//	int scene = GameViewWindow::getSceneNum();
		//	ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetSaveJSONPath(scene));
		//}

		//else
		//{
		//	ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetEntitiesJSONPath());
		//}
	}

	// this handles the options button
	else if (entityId == "optionsButton" || entityId == "pauseOptionsButton")
	{
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
		}
	}

	// this handles the how to play button
	else if (entityId == "tutorialButton" || entityId == "pauseTutorialButton")
	{
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

		if (GLFWFunctions::tutorialMenuCount < 1)
		{
			GLFWFunctions::tutorialMenuCount++;
			ecsCoordinator.LoadTutorialMenuFromJSON(ecsCoordinator, FilePathManager::GetTutorialJSONPath());
		}
	}

	// this handles the closing of the pause menu button and resume level button
	else if (entityId == "closePauseMenu" || entityId == "resumeButton")
	{
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

	// this handles the closing of the options menu button
	else if (entityId == "closeOptionsMenu")
	{
		std::unordered_set<std::string> optionsMenuEntityNames = {
			"optionsMenuBg", "closeOptionsMenu", "confirmButton",
			"sfxAudio", "musicAudio", "sfxSoundbarBase",
			"musicSoundbarBase", "sfxSoundbarArrow", "musicSoundbarArrow", 
			"sfxNotch0", "sfxNotch1", "sfxNotch2", "sfxNotch3",
			"sfxNotch4", "sfxNotch5", "sfxNotch6", "sfxNotch7",
			"sfxNotch8", "sfxNotch9", "musicNotch0", "musicNotch1",
			"musicNotch2", "musicNotch3", "musicNotch4",
			"musicNotch5", "musicNotch6", "musicNotch7",
			"musicNotch8", "musicNotch9"
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
	}

	else if (entityId == "closeTutorialMenu")
	{
		for (auto currEntity : allEntities)
		{
			if (ecsCoordinator.getEntityID(currEntity) == "tutorialBaseBg" ||
				ecsCoordinator.getEntityID(currEntity) == "closeTutorialMenu" ||
				ecsCoordinator.getEntityID(currEntity) == "pageCounter" ||
				ecsCoordinator.getEntityID(currEntity) == "nextTutorialPage" ||
				ecsCoordinator.getEntityID(currEntity) == "previousTutorialPage")
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

	else if (entityId == "nextTutorialPage")
	{
		GLFWFunctions::tutorialCurrentPage++;
	}

	else if (entityId == "previousTutorialPage")
	{
		GLFWFunctions::tutorialCurrentPage--;
	}

	// this handles the logic for exiting the level and goes back to the main menu button
	else if (entityId == "pauseQuitButton")
	{
		// destroy all the entities in the current scene
		for (auto currEntity : allEntities)
		{
			ecsCoordinator.destroyEntity(currEntity);
		}

		// decrement the pause menu count and load the main menu back into the scene
		GLFWFunctions::pauseMenuCount--;
		GameViewWindow::setSceneNum(-1);
		ecsCoordinator.LoadMainMenuFromJSON(ecsCoordinator, FilePathManager::GetMainMenuJSONPath());
	}

	// this handles the logic code for the sfx and music soundbarBase buttons
	else if (entityId == "sfxSoundbarBase" || entityId == "musicSoundbarBase")
	{
		// getting the window's width, height and cursor position x and y values
		double mouseX{}, mouseY{};
		int windowWidth{}, windowHeight{};
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

	// this handles the logic for the confirm button
	else if (entityId == "confirmButton")
	{
		std::unordered_set<std::string> optionsMenuEntityNames = {
			"optionsMenuBg", "closeOptionsMenu", "confirmButton",
			"sfxAudio", "musicAudio", "sfxSoundbarBase",
			"musicSoundbarBase", "sfxSoundbarArrow", "musicSoundbarArrow",
			"sfxNotch0", "sfxNotch1", "sfxNotch2", "sfxNotch3",
			"sfxNotch4", "sfxNotch5", "sfxNotch6", "sfxNotch7",
			"sfxNotch8", "sfxNotch9", "musicNotch0", "musicNotch1",
			"musicNotch2", "musicNotch3", "musicNotch4",
			"musicNotch5", "musicNotch6", "musicNotch7",
			"musicNotch8", "musicNotch9"
		};

		// initializing sfxPercentage and musicPercentage variables
		float sfxPercentage = AudioSystem::sfxPercentage;
		float musicPercentage = AudioSystem::musicPercentage;

		// save the new audio arrow (for both sfx and music) position x to the options menu JSON file
		ecsCoordinator.SaveOptionsSettingsToJSON(ecsCoordinator, FilePathManager::GetOptionsMenuJSONPath());
		// save the sfx and music percentages to the audio settings JSON file
		audioSystem.saveAudioSettingsToJSON(FilePathManager::GetAudioSettingsJSONPath(), sfxPercentage, musicPercentage);

		//change on audio side as well
		audioSystem.setGenVol(musicPercentage);
		audioSystem.setBgmVol(musicPercentage);
		audioSystem.setSfxVol(sfxPercentage);

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

