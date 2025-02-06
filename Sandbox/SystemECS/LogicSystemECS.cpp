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

void MouseBehaviour::onMouseClick(GLFWwindow* window, double mouseX, double mouseY)
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();

	for (auto& entity : allEntities)
	{
		if (ecsCoordinator.hasComponent<ButtonComponent>(entity))
		{
			//check if entity is visible
			if (layerManager.getEntityVisibility(entity))
			{
				TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

				if (mouseIsOverButton(mouseX, mouseY, transform))
				{
				handleButtonClick(window, entity);

				std::string entityId = ecsCoordinator.getEntityID(entity);
				
				if (entityId == "sfxSoundbarBase" || entityId == "musicSoundbarBase")
				{
					isDragging = true;
				}
				}
			}
		}
	}
}

void MouseBehaviour::onMouseDrag(GLFWwindow* window, double mouseX, double mouseY)
{
	if (!isDragging)
	{
		return;
	}

	auto allEntities = ecsCoordinator.getAllLiveEntities();

	int windowWidth{}, windowHeight{};
	glfwGetWindowSize(GLFWFunctions::pWindow, &windowWidth, &windowHeight);
	float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);

	std::string soundbarArrow = (getSoundbarId() == "sfxSoundbarBase") ? "sfxSoundbarArrow" :
								(getSoundbarId() == "musicSoundbarBase") ? "musicSoundbarArrow" : "";

	if (!soundbarArrow.empty())
	{
		TransformComponent soundbarTransform{};

		for (auto& entity : allEntities)
		{
			std::string entityId = ecsCoordinator.getEntityID(entity);
			if (entityId == getSoundbarId())
			{
				soundbarTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
				break;
			}
		}

		float soundbarLeft = soundbarTransform.position.GetX() - (soundbarTransform.scale.GetX() / 2.12f);
		float soundbarRight = soundbarTransform.position.GetX() + (soundbarTransform.scale.GetX() / 2.1f);

		if (cursorXCentered >= soundbarLeft && cursorXCentered <= soundbarRight)
		{
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

void MouseBehaviour::onMouseHover(double mouseX, double mouseY)
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	GLFWFunctions::isHovering = false;
	setHoveredButton("");

	for (auto& entity : allEntities)
	{
		if (ecsCoordinator.hasComponent<ButtonComponent>(entity))
		{
			//check if entity is visible
			if (layerManager.getEntityVisibility(entity))
			{
				TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
				//ButtonComponent& button = ecsCoordinator.getComponent<ButtonComponent>(entity);

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

	if (!GLFWFunctions::isHovering)
	{
		glfwSetCursor(GLFWFunctions::pWindow, nullptr);
	}
}

bool MouseBehaviour::mouseIsOverButton(double mouseX, double mouseY, TransformComponent& transform)
{
	float const scalar = 0.85f;
    float buttonLeft = transform.position.GetX() - transform.scale.GetX() * scalar / 1.8f;
    float buttonRight = transform.position.GetX() + transform.scale.GetX() * scalar / 1.85f;
    float buttonTop = transform.position.GetY() + transform.scale.GetY() * scalar / 2.f;
    float buttonBottom = transform.position.GetY() - transform.scale.GetY() * scalar / 2.f;

	return (mouseX >= static_cast<double>(buttonLeft) && mouseX <= static_cast<double>(buttonRight) && mouseY >= static_cast<double>(buttonBottom) && mouseY <= static_cast<double>(buttonTop));
}

void MouseBehaviour::handleButtonClick(GLFWwindow* window, Entity entity)
{
	std::string entityId = ecsCoordinator.getEntityID(entity);
	auto allEntities = ecsCoordinator.getAllLiveEntities();
	setSoundbarId("");

	if (entityId == "quitButton" || entityId == "quitWindowButton")
	{
		audioSystem.playSoundEffect("UI_ButtonClick.wav");

		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	else if (entityId == "retryButton")
	{
		for (auto currEntity : allEntities)
		{
			ecsCoordinator.destroyEntity(currEntity);
		}

		audioSystem.playSoundEffect("UI_ButtonClick.wav");
		GLFWFunctions::gameOver = false;
		GLFWFunctions::gamePaused = false;
		GLFWFunctions::pauseMenuCount = 0;
		GLFWFunctions::optionsMenuCount = 0;

		//ecsCoordinator.test5();

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

	else if (entityId == "startButton")
	{
		GLFWFunctions::gamePaused = false;
		GLFWFunctions::optionsMenuCount = 0;
		int levelOneScene = 1;

		for (auto currEntity : allEntities)
		{
			ecsCoordinator.destroyEntity(currEntity);
		}

		audioSystem.playSoundEffect("UI_ButtonClick.wav");
		//set scene to play cutscene
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

	else if (entityId == "optionsButton" || entityId == "pauseOptionsButton")
	{
		if (GLFWFunctions::pauseMenuCount == 1)
		{
			for (auto currEntity : allEntities)
			{
				if (ecsCoordinator.getEntityID(currEntity) == "pauseMenuBg" ||
					ecsCoordinator.getEntityID(currEntity) == "closePauseMenu" ||
					ecsCoordinator.getEntityID(currEntity) == "resumeButton" ||
					ecsCoordinator.getEntityID(currEntity) == "pauseOptionsButton" ||
					ecsCoordinator.getEntityID(currEntity) == "pauseTutorialButton" ||
					ecsCoordinator.getEntityID(currEntity) == "pauseQuitButton")
				{
					ecsCoordinator.destroyEntity(currEntity);
				}
			}

			GLFWFunctions::pauseMenuCount--;
		}

		if (GLFWFunctions::optionsMenuCount < 1)
		{
			GLFWFunctions::optionsMenuCount++;
			ecsCoordinator.LoadOptionsMenuFromJSON(ecsCoordinator, FilePathManager::GetOptionsMenuJSONPath());
		}
	}

	else if (entityId == "closePauseMenu" || entityId == "resumeButton")
	{
		for (auto currEntity : allEntities)
		{
			if (ecsCoordinator.getEntityID(currEntity) == "pauseMenuBg" || 
				ecsCoordinator.getEntityID(currEntity) == "closePauseMenu" ||
				ecsCoordinator.getEntityID(currEntity) == "resumeButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseOptionsButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseTutorialButton" ||
				ecsCoordinator.getEntityID(currEntity) == "pauseQuitButton")
			{
				ecsCoordinator.destroyEntity(currEntity);
			}
		}

		GLFWFunctions::gamePaused = false;
		GLFWFunctions::pauseMenuCount--;
	}

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

		for (auto currEntity : allEntities)
		{
			if (optionsMenuEntityNames.count(ecsCoordinator.getEntityID(currEntity)))
			{
				ecsCoordinator.destroyEntity(currEntity);
			}
		}

		GLFWFunctions::optionsMenuCount--;

		if (GameViewWindow::getSceneNum() > -1 && GLFWFunctions::pauseMenuCount < 1)
		{
			ecsCoordinator.LoadPauseMenuFromJSON(ecsCoordinator, FilePathManager::GetPauseMenuJSONPath());
			GLFWFunctions::pauseMenuCount++;
		}

		GLFWFunctions::gamePaused = true;
	}

	else if (entityId == "pauseQuitButton")
	{
		for (auto currEntity : allEntities)
		{
			ecsCoordinator.destroyEntity(currEntity);
		}

		GLFWFunctions::pauseMenuCount--;
		ecsCoordinator.LoadMainMenuFromJSON(ecsCoordinator, FilePathManager::GetMainMenuJSONPath());
	}

	else if (entityId == "sfxSoundbarBase" || entityId == "musicSoundbarBase")
	{
		double mouseX{}, mouseY{};
		int windowWidth{}, windowHeight{};
		glfwGetCursorPos(GLFWFunctions::pWindow, &mouseX, &mouseY);
		glfwGetWindowSize(GLFWFunctions::pWindow, &windowWidth, &windowHeight);
		setSoundbarId(entityId);

		float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);
		std::string audioArrowId = (entityId == "sfxSoundbarBase") ? "sfxSoundbarArrow" : "musicSoundbarArrow";

		for (auto& currEntity : allEntities)
		{
			if (ecsCoordinator.getEntityID(currEntity) == audioArrowId)
			{
				TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(currEntity);
				transform.position.SetX(cursorXCentered);
				break;
			}
		}
	}

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

		float sfxPercentage = AudioSystem::sfxPercentage;
		float musicPercentage = AudioSystem::musicPercentage;

		ecsCoordinator.SaveOptionsSettingsToJSON(ecsCoordinator, FilePathManager::GetOptionsMenuJSONPath());
		audioSystem.saveAudioSettingsToJSON(FilePathManager::GetAudioSettingsJSONPath(), sfxPercentage, musicPercentage);

		std::cout << "SFX: " << sfxPercentage << std::endl;
		std::cout << "Music: " << musicPercentage << std::endl;

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

		GLFWFunctions::optionsMenuCount--;

		if (GameViewWindow::getSceneNum() > -1 && GLFWFunctions::pauseMenuCount < 1)
		{
			ecsCoordinator.LoadPauseMenuFromJSON(ecsCoordinator, FilePathManager::GetPauseMenuJSONPath());
			GLFWFunctions::pauseMenuCount++;
		}

		GLFWFunctions::gamePaused = true;
	}
}

MouseBehaviour::~MouseBehaviour()
{
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

