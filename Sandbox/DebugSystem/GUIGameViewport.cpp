/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIGameViewport.cpp
@brief  :  This file contains the function definition of ImGui game viewport system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui game viewport window to capture game scene in real time during debug mode
		- Integrated game viewport camera controls for zooming and panning current game scene in real time

*Ian Loi (ian.loi) :
		- Integrated serialization & deserialization functions to initialize variables from json file, which
		  allows saving and loading feature in the level editor

*Liu YaoTing (yaoting.liu) :
		- Integrated save and load function for multiple files
		- Updated "Pause", "Zoom", and "Pan" buttons

*Chu Wei Yuan (c.weiyuan):
		- Integrated layering system

File Contributions: Lew Zong Han Owen (58%)
					Ian Loi           (9%)
					Liu YaoTing       (28%)
					Chu Wei Yuan Joel (5%)

/*_______________________________________________________________________________________________________________*/
#define _USE_MATH_DEFINES
#include <cmath>
#include "GUIGameViewport.h"
#include "GlfwFunctions.h"
#include "GUIConsole.h"
#include "Debug.h"
#include "GlobalCoordinator.h"
#include "GUIInspector.h"
#include "BehaviourComponent.h"
#include "FilterComponent.h"
#include "MovPlatformComponent.h"
#include "LogicSystemECS.h"
#include "PlayerBehaviour.h"
#include "EnemyBehaviour.h"
#include "CollectableBehaviour.h"
#include "EffectPumpBehaviour.h"
#include "ExitBehaviour.h"
#include "BackgroundComponent.h"
#include "PlatformBehaviour.h"
#include "MovPlatformBehaviour.h"
#include "FilterBehaviour.h"
#include "UIComponent.h"

//Variables for GameViewWindow
int GameViewWindow::viewportHeight;
int GameViewWindow::viewportWidth;
ImVec2 GameViewWindow::viewportPos;
GLuint GameViewWindow::viewportTexture;
ImVec2 GameViewWindow::windowSize;
ImVec2 GameViewWindow::aspectSize;
ImVec2 GameViewWindow::globalMousePos;
ImVec2 GameViewWindow::applicationCenter;
ImVec2 GameViewWindow::renderPos;

bool insideViewport = false;
bool zoomTestFlag = false;
int GameViewWindow::saveNum;
int GameViewWindow::fileNum;
bool GameViewWindow::clickedZoom = false;

float GameViewWindow::currentZoom = 0.2f;
float GameViewWindow::zoomLevel;
float GameViewWindow::MIN_ZOOM;
float GameViewWindow::MAX_ZOOM;

float GameViewWindow::headerHeight;
float GameViewWindow::optionButtonHeight;
float GameViewWindow::scrollY;

bool GameViewWindow::clickedScreenPan = false;
bool GameViewWindow::isDragging = false;
ImVec2 GameViewWindow::initialMousePos;
ImVec2 GameViewWindow::mouseDragDist;
float GameViewWindow::mouseDragSpeed;
ImVec2 GameViewWindow::accumulatedMouseDragDist;
ImVec2 GameViewWindow::currentMouseDragDist;

float GameViewWindow::aspectRatioXScale;
float GameViewWindow::aspectRatioYScale;
float GameViewWindow::aspectRatioWidth;
float GameViewWindow::aspectRatioHeight;

bool GameViewWindow::isSelectingSaveFile;
bool GameViewWindow::isSelectingFile = false;
bool GameViewWindow::saveFileChosen = false;
bool GameViewWindow::loadFileChosen = false;
int GameViewWindow::saveCount;
int GameViewWindow::saveLimit;
float GameViewWindow::lastPosX;
float GameViewWindow::saveWindowWidth;
float GameViewWindow::fileWindowWidth;
float GameViewWindow::slotWindowWidth;
float GameViewWindow::clearSlotWindowWidth;
float GameViewWindow::saveWindowHeight;
float GameViewWindow::fileWindowHeight;
float GameViewWindow::optionsButtonPadding;
int GameViewWindow::scene;
float GameViewWindow::initialZoom;
float GameViewWindow::mouseWheelScaleFactor;
int GameViewWindow::objectCounter = 1;

//Initialize game viewport system
void GameViewWindow::Initialise() {
	LoadViewportConfigFromJSON(FilePathManager::GetIMGUIViewportJSONPath());
	LoadSceneFromJSON(FilePathManager::GetSceneJSONPath());
	//currentZoom = cameraSystem.getCameraZoom();
}
bool GameViewWindow::isPaused = false;
//Handle viewport setup, processing and rendering
void GameViewWindow::Update() {
	SetupViewportTexture();

	aspectSize = GetLargestSizeForViewport();
	windowSize = ImGui::GetContentRegionAvail();
	globalMousePos = ImGui::GetMousePos();
	applicationCenter = ImGui::GetMainViewport()->GetCenter();
	renderPos = GetCenteredPosForViewport(aspectSize);
	headerHeight = ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetFontSize();
	optionButtonHeight = ImGui::GetFrameHeight();

	ImGui::Begin("Game Viewport");

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetStyle().ItemSpacing.y * 2));

	if (ImGui::Button("Save")) {
		isSelectingSaveFile = true;
	}

	if (isSelectingSaveFile) {
		ImGui::OpenPopup("Choose save files");
	}

	ImGui::SetNextWindowPos(applicationCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	// Create popup modal window for loading of save files
	if (ImGui::BeginPopupModal("Choose save files", &isSelectingSaveFile, ImGuiWindowFlags_AlwaysAutoResize)) {

		ImGui::BeginChild("SaveFilesList", ImVec2(saveWindowWidth, saveWindowHeight), true);

		if (ImGui::Button("Slot 1", ImVec2(slotWindowWidth, fileWindowHeight))) {
			saveNum = 1;
			saveFileChosen = true;
			isSelectingSaveFile = false;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine(0, optionsButtonPadding);
		if (ImGui::Button("Clear 1", ImVec2(clearSlotWindowWidth, fileWindowHeight))) {
			fileNum = 1;
			std::string saveFile = GenerateSaveJSONFile(fileNum);
			nlohmann::ordered_json jsonData;
			jsonData["entities"] = nlohmann::ordered_json::array();
			jsonData["entities"].push_back(nlohmann::ordered_json{
				{"id", "placeholderentity"}
				});
			std::ofstream outputFile(saveFile);
			if (outputFile.is_open()) {
				outputFile << jsonData.dump(2);
				outputFile.close();
			}

			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
		}

		if (ImGui::Button("Slot 2", ImVec2(slotWindowWidth, fileWindowHeight))) {
			saveNum = 2;
			saveFileChosen = true;
			isSelectingSaveFile = false;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine(0, optionsButtonPadding);
		if (ImGui::Button("Clear 2", ImVec2(clearSlotWindowWidth, fileWindowHeight))) {
			fileNum = 2;
			std::string saveFile = GenerateSaveJSONFile(fileNum);
			nlohmann::ordered_json jsonData;
			jsonData["entities"] = nlohmann::ordered_json::array();
			jsonData["entities"].push_back(nlohmann::ordered_json{
				{"id", "placeholderentity"}
				});
			std::ofstream outputFile(saveFile);
			if (outputFile.is_open()) {
				outputFile << jsonData.dump(2);
				outputFile.close();
			}

			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
		}

		if (ImGui::Button("Slot 3", ImVec2(slotWindowWidth, fileWindowHeight))) {
			saveNum = 3;
			saveFileChosen = true;
			isSelectingSaveFile = false;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine(0, optionsButtonPadding);
		if (ImGui::Button("Clear 3", ImVec2(clearSlotWindowWidth, fileWindowHeight))) {
			fileNum = 3;
			std::string saveFile = GenerateSaveJSONFile(fileNum);
			nlohmann::ordered_json jsonData;
			jsonData["entities"] = nlohmann::ordered_json::array();
			jsonData["entities"].push_back(nlohmann::ordered_json{
				{"id", "placeholderentity"}
				});
			std::ofstream outputFile(saveFile);
			if (outputFile.is_open()) {
				outputFile << jsonData.dump(2);
				outputFile.close();
			}

			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
		}

		if (ImGui::Button("Slot 4", ImVec2(slotWindowWidth, fileWindowHeight))) {
			saveNum = 4;
			saveFileChosen = true;
			isSelectingSaveFile = false;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine(0, optionsButtonPadding);
		if (ImGui::Button("Clear 4", ImVec2(clearSlotWindowWidth, fileWindowHeight))) {
			fileNum = 4;
			std::string saveFile = GenerateSaveJSONFile(fileNum);
			nlohmann::ordered_json jsonData;
			jsonData["entities"] = nlohmann::ordered_json::array();
			jsonData["entities"].push_back(nlohmann::ordered_json{
				{"id", "placeholderentity"}
				});
			std::ofstream outputFile(saveFile);
			if (outputFile.is_open()) {
				outputFile << jsonData.dump(2);
				outputFile.close();
			}

			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
		}

		if (ImGui::Button("Slot 5", ImVec2(slotWindowWidth, fileWindowHeight))) {
			saveNum = 5;
			saveFileChosen = true;
			isSelectingSaveFile = false;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine(0, optionsButtonPadding);
		if (ImGui::Button("Clear 5", ImVec2(clearSlotWindowWidth, fileWindowHeight))) {
			fileNum = 5;
			std::string saveFile = GenerateSaveJSONFile(fileNum);
			nlohmann::ordered_json jsonData;
			jsonData["entities"] = nlohmann::ordered_json::array();
			jsonData["entities"].push_back(nlohmann::ordered_json{
				{"id", "placeholderentity"}
				});
			std::ofstream outputFile(saveFile);
			if (outputFile.is_open()) {
				outputFile << jsonData.dump(2);
				outputFile.close();
			}

			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
		}


		if (saveFileChosen) {
			std::string saveFile = GenerateSaveJSONFile(saveNum);

			// Create base JSON structure using ordered_json consistently
			nlohmann::ordered_json jsonData;
			jsonData["entities"] = nlohmann::ordered_json::array();

			// Add placeholder entity using ordered_json
			jsonData["entities"].push_back(nlohmann::ordered_json{
				{"id", "placeholderentity"},
				{"textureId", ""}
				});

			// Save all currently live entities
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				std::string entityId = ecsCoordinator.getEntityID(entity);
				std::string textureId = ecsCoordinator.getTextureID(entity);
				if (entityId != "placeholderentity") {
					TransformComponent transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					auto entityJson = AddNewEntityToJSON(transform, entityId, textureId, ecsCoordinator, entity);
					jsonData["entities"].push_back(entityJson);
				}
			}

			// Clear newEntities since they're already saved
			DebugSystem::newEntities->clear();

			// Save to file
			std::ofstream outputFile(saveFile);
			if (outputFile.is_open()) {
				outputFile << jsonData.dump(2);
				outputFile.close();
			}

			saveFileChosen = false;
			isSelectingSaveFile = false;
			//ImGui::CloseCurrentPopup();
		}


		ImGui::EndChild();

		ImGui::EndPopup();
	}

	ImGui::SameLine(0, 5);

	if (ImGui::Button("Load")) {
		isSelectingFile = true;
	}

	if (isSelectingFile) {
		ImGui::OpenPopup("Load save files");
	}

	ImGui::SetNextWindowPos(applicationCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	// Create popup modal window for loading of save files
	if (ImGui::BeginPopupModal("Load save files", &isSelectingFile, ImGuiWindowFlags_AlwaysAutoResize)) {

		ImGui::BeginChild("SaveFilesList", ImVec2(saveWindowWidth, saveWindowHeight), true);

		if (ImGui::Button("Original File", ImVec2(fileWindowWidth, fileWindowHeight))) {
			scene = 0;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			SaveSceneToJSON(FilePathManager::GetSceneJSONPath());
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				ecsCoordinator.destroyEntity(entity);
			}
			ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetEntitiesJSONPath());
			isSelectingFile = false;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Save 1", ImVec2(fileWindowWidth, fileWindowHeight))) {
			saveNum = 1;
			scene = saveNum;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			SaveSceneToJSON(FilePathManager::GetSceneJSONPath());
			loadFileChosen = true;
			isSelectingFile = false;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Save 2", ImVec2(fileWindowWidth, fileWindowHeight))) {
			saveNum = 2;
			scene = saveNum;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			SaveSceneToJSON(FilePathManager::GetSceneJSONPath());
			loadFileChosen = true;
			isSelectingFile = false;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Save 3", ImVec2(fileWindowWidth, fileWindowHeight))) {
			saveNum = 3;
			scene = saveNum;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			SaveSceneToJSON(FilePathManager::GetSceneJSONPath());
			loadFileChosen = true;
			isSelectingFile = false;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Save 4", ImVec2(fileWindowWidth, fileWindowHeight))) {
			saveNum = 4;
			scene = saveNum;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			SaveSceneToJSON(FilePathManager::GetSceneJSONPath());
			loadFileChosen = true;
			isSelectingFile = false;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Save 5", ImVec2(fileWindowWidth, fileWindowHeight))) {
			saveNum = 5;
			scene = saveNum;
			GLFWFunctions::gamePaused = false;
			GLFWFunctions::pauseMenuCount = 0;
			GLFWFunctions::optionsMenuCount = 0;
			SaveSceneToJSON(FilePathManager::GetSceneJSONPath());
			loadFileChosen = true;
			isSelectingFile = false;
			ImGui::CloseCurrentPopup();
		}

		if (loadFileChosen) {
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				ecsCoordinator.destroyEntity(entity);
			}

			ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetSaveJSONPath(saveNum));
			loadFileChosen = false;
		}

		ImGui::EndChild();

		ImGui::EndPopup();
	}

	ImGui::SameLine(0, optionsButtonPadding);

	// Add pause button to viewport
	if (ImGui::Button(isPaused ? "Resume" : "Pause") || ImGui::IsKeyPressed(ImGuiKey_Q)) {
		Inspector::selectedEntityID = -1;
		Inspector::draggedEntityID = -1;
		Inspector::isSelectingEntity = false;
		TogglePause();
	}

	ImGui::PopStyleVar();

	ImGui::SameLine(0, optionsButtonPadding);

	if (/*ImGui::Button(clickedZoom ? "UnZoom" : "Zoom") || */ImGui::IsKeyPressed(ImGuiKey_W)) {
		clickedZoom = !clickedZoom;
	}

	ImGui::SameLine(0, optionsButtonPadding);

	if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
		GLFWFunctions::allow_camera_movement = true;
	}
	else {
		GLFWFunctions::allow_camera_movement == false;
	}

	//if (/*ImGui::Button(clickedScreenPan ? "UnPan" : "Pan") || */ImGui::IsKeyPressed(ImGuiKey_E)) {
	//	if (clickedScreenPan) {
	//		// Clicking "UnPan" - enable camera movement
	//		GLFWFunctions::allow_camera_movement = true;
	//	}
	//	else if (!clickedScreenPan && GLFWFunctions::allow_camera_movement == false) {
	//		// Clicking "Pan" - disable camera movement
	//		GLFWFunctions::allow_camera_movement = true;
	//	}
	//	
	//	clickedScreenPan = !clickedScreenPan;
	//}



	ImGui::SameLine(0, optionsButtonPadding);


	if (ImGui::Button("Reset Perspective")) {
		myMath::Vector2D initialCamPos{};

		initialCamPos = { 0,0 };

		cameraSystem.setCameraPosition(initialCamPos);
		// Reset camera zoom to default value
		cameraSystem.setCameraZoom(currentZoom);
	}

	viewportPos = ImGui::GetWindowPos();

	if (GameViewWindow::IsPointInViewport(globalMousePos.x, globalMousePos.y))
		insideViewport = true;
	else
		insideViewport = false;

	ImGuiIO& io = ImGui::GetIO();
	scrollY = io.MouseWheel;

	if (insideViewport && clickedZoom) {
		currentZoom = cameraSystem.getCameraZoom() + scrollY * mouseWheelScaleFactor;

		// Clamp the new zoom value between min and max
		currentZoom = std::clamp(currentZoom, MIN_ZOOM, MAX_ZOOM);

		cameraSystem.setCameraZoom(currentZoom);
	}

	CaptureMainWindow();


	ImGui::SetCursorPos(renderPos);
	ImTextureID textureID = (ImTextureID)(intptr_t)viewportTexture;
	ImGui::Image(textureID, aspectSize, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payloadTex = ImGui::AcceptDragDropPayload("TEXTURE_PAYLOAD")) {
			const char* assetName = (const char*)payloadTex->Data;
			createDropEntity(assetName, TEXTURE);
			std::cout << "Dropped Texture: " << assetName << std::endl;

		}
		else if (const ImGuiPayload* payloadShdr = ImGui::AcceptDragDropPayload("SHADER_PAYLOAD")) {
			const char* assetName = (const char*)payloadShdr->Data;
			std::cout << "Dropped Shader: " << assetName << std::endl;

		}
		else if (const ImGuiPayload* payloadFont = ImGui::AcceptDragDropPayload("FONT_PAYLOAD")) {
			const char* assetName = (const char*)payloadFont->Data;
			createDropEntity(assetName, FONT);
			std::cout << "Dropped Font: " << assetName << std::endl;

		}
		else if (const ImGuiPayload* payloadAud = ImGui::AcceptDragDropPayload("PREFAB_PAYLOAD")) {
			const char* assetName = (const char*)payloadAud->Data;
			createDropEntity(assetName, PREFAB);
			std::cout << "Dropped prefab: " << assetName << std::endl;
		}
		ImGui::EndDragDropTarget();
	}

	Console::GetLog() << "zoom" << currentZoom << std::endl;

	ImGui::End();
}
//Clean up resources
void GameViewWindow::Cleanup() {
	// Then cleanup viewport texture
	if (viewportTexture != 0) {
		glDeleteTextures(1, &viewportTexture);
		viewportTexture = 0;
	}
}
//Set up Opengl texture to store game scene
void GameViewWindow::SetupViewportTexture() {
	if (viewportTexture != 0) {
		glDeleteTextures(1, &viewportTexture);
	}

	glGenTextures(1, &viewportTexture);
	viewportWidth = GLFWFunctions::windowWidth;
	viewportHeight = GLFWFunctions::windowHeight;
	glBindTexture(GL_TEXTURE_2D, viewportTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GLFWFunctions::windowWidth, GLFWFunctions::windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GameViewWindow::CaptureMainWindow() {

	glBindTexture(GL_TEXTURE_2D, viewportTexture);

	GLint previousBuffer;
	glGetIntegerv(GL_READ_BUFFER, &previousBuffer);

	glReadBuffer(GL_BACK);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, GLFWFunctions::windowWidth, GLFWFunctions::windowHeight, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Function scale real time game scene to the size of the game viewport window
ImVec2 GameViewWindow::GetLargestSizeForViewport()
{
	// Calculate aspect ratio
	float targetAspectRatio = (float)GLFWFunctions::windowWidth / (float)GLFWFunctions::windowHeight;

	// Get available space in the viewport window
	float pauseButtonHeight = ImGui::GetFrameHeight();
	float padding = ImGui::GetStyle().ItemSpacing.y * 2; // Add padding between elements

	// Calculate total UI overhead
	float uiOverhead = headerHeight + pauseButtonHeight + padding;

	float availableWidth = windowSize.x;
	float availableHeight = windowSize.y - uiOverhead;

	// Calculate dimensions that maintain aspect ratio
	float width = availableWidth;
	float height = width / targetAspectRatio;

	// If height is too big, scale based on height instead
	if (height > availableHeight) {
		height = availableHeight;
		width = height * targetAspectRatio;
	}

	// Apply zoom while ensuring we don't exceed available space
	width *= zoomLevel;
	height *= zoomLevel;

	// Clamp dimensions to available space
	if (height > availableHeight) {
		float scale = availableHeight / height;
		width *= scale;
		height = availableHeight;
	}

	return ImVec2(width, height);

}

//Function to translate the real time game scene to the center of the game viewport window
ImVec2 GameViewWindow::GetCenteredPosForViewport(ImVec2 size)
{
	// Calculate UI element heights and padding
	float pauseButtonHeight = ImGui::GetFrameHeight();
	float padding = ImGui::GetStyle().ItemSpacing.y * 2;

	// Calculate vertical starting position after header and pause button with padding
	float startY = headerHeight + pauseButtonHeight + padding;

	// Center horizontally
	float viewportX = (windowSize.x - size.x) * 0.5f;

	// Position vertically after UI elements with proper spacing
	float remainingHeight = windowSize.y - startY;
	float viewportY = startY + (remainingHeight - size.y) * 0.5f;

	// Handle panning logic
	if (/*clickedScreenPan && */insideViewport && ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
		if (!isDragging) {
			isDragging = true;
			initialMousePos = globalMousePos;
		}

		currentMouseDragDist = { initialMousePos.x - globalMousePos.x, initialMousePos.y - globalMousePos.y };

		// Move camera position
		myMath::Vector2D camPos = cameraSystem.getCameraPosition();

		float zoomAdjustedSpeed = mouseDragSpeed * (2.0f / currentZoom);
		camPos.SetX(camPos.GetX() + (currentMouseDragDist.x - mouseDragDist.x) * zoomAdjustedSpeed);
		camPos.SetY(camPos.GetY() - (currentMouseDragDist.y - mouseDragDist.y) * zoomAdjustedSpeed);
		cameraSystem.setCameraPosition(camPos);

		mouseDragDist = currentMouseDragDist;
	}
	else {
		isDragging = false;
		mouseDragDist = { 0.0f, 0.0f };
	}

	return ImVec2(viewportX, viewportY);

}
//Checks if mouse coordinates are in viewport
bool GameViewWindow::IsPointInViewport(double x, double y) {
	return (x - viewportPos.x <= windowSize.x && x - viewportPos.x >= 0
		&& y - viewportPos.y <= windowSize.y && y - viewportPos.y >= 0);
}
//Save viewport configuration to JSON
void GameViewWindow::SaveViewportConfigToJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();

	serializer.WriteInt(viewportWidth, "GUIViewport.viewportWidth", filename);
	serializer.WriteInt(viewportHeight, "GUIViewport.viewportHeight", filename);

	serializer.WriteFloat(viewportPos.x, "GUIViewport.viewportPos.x", filename);
	serializer.WriteFloat(viewportPos.y, "GUIViewport.viewportPos.y", filename);

	serializer.WriteUnsignedInt(viewportTexture, "GUIViewport.viewportTexture", filename);

	serializer.WriteFloat(zoomLevel, "GUIViewport.zoomLevel", filename);

	serializer.WriteFloat(MIN_ZOOM, "GUIViewport.minZoom", filename);
	serializer.WriteFloat(MAX_ZOOM, "GUIViewport.maxZoom", filename);
}

//Create entity from drag and drop
void GameViewWindow::createDropEntity(const char* assetName, Specifier specifier) {
	if (specifier == PREFAB)
	{
		std::string assetPath = assetsManager.GetPrefabPath(assetName);
		LoadPrefabFromJSON(assetName, assetPath);
		return;
	}


	Entity dropEntity = ecsCoordinator.createEntity();

	auto logicSystemRef = ecsCoordinator.getSpecificSystem<LogicSystemECS>();

	//default add to layer 0
	layerManager.addEntityToLayer(0, dropEntity);

	TransformComponent transform;
	transform.position = { Inspector::getMouseWorldPos().x, Inspector::getMouseWorldPos().y };
	transform.scale = { 100.0f, 100.0f };
	transform.orientation = { 0.0f, 0.0f };

	ecsCoordinator.addComponent(dropEntity, transform);

	BehaviourComponent behaviour;
	behaviour.none = true;
	ecsCoordinator.addComponent(dropEntity, behaviour);

	JSONSerializer serializer;

	// Add the appropriate components based on the specifier
	switch (specifier) {
	case TEXTURE:
		{
			if (strcmp(assetName, "goldfish") == 0) {
				EnemyComponent enemy;
				serializer.ReadObject(enemy.isEnemy, assetName, "entities.enemy.isEnemy");

				PhysicsComponent physics;

				ecsCoordinator.addComponent(dropEntity, enemy);
				ecsCoordinator.addComponent(dropEntity, physics);
			}
			if (strcmp(assetName, "mossball") == 0) {
				PlayerComponent player;
				serializer.ReadObject(player.isPlayer, assetName, "entities.player.isPlayer");
				ecsCoordinator.addComponent(dropEntity, player);

				PhysicsComponent physics;
				ecsCoordinator.addComponent(dropEntity, physics);
			}

			break;
		}
	case FONT:
		{
			FontComponent font;
			font.fontId = assetName;
			font.text = "Temporary";
			font.textScale = 1.0f;
			font.textBoxWidth = 300.0f;
			font.color = { 1.0f, 1.0f, 1.0f };
			ecsCoordinator.addComponent(dropEntity, font);
			break;
		}
	}
	std::string entityId = "Object" + std::to_string(objectCounter);
	objectCounter++;

	ecsCoordinator.setTextureID(dropEntity, assetName);
	ecsCoordinator.setEntityID(dropEntity, entityId);

}

std::string GameViewWindow::GenerateSaveJSONFile(int& saveNumber)
{
	std::string execPath = FilePathManager::GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\Sandbox\\assets\\json\\save" + std::to_string(saveNumber) + ".json";
	std::string sourceFilePath;

	sourceFilePath = FilePathManager::GetEntitiesJSONPath();

	nlohmann::json entitiesJSON;

	std::ifstream sourceFile(sourceFilePath);

	if (sourceFile.is_open())
	{
		sourceFile >> entitiesJSON;
		sourceFile.close();
	}

	std::ofstream outFile(jsonPath);
	if (!outFile.is_open()) {
		std::cout << "Error: could not create file " << jsonPath << std::endl;
		return "";
	}
	outFile.close();

	return jsonPath;
}

nlohmann::ordered_json GameViewWindow::AddNewEntityToJSON(TransformComponent& transform, std::string const& entityId, std::string const& textureId, ECSCoordinator& ecs, Entity& entity)
{
	auto logicSystemRef = ecsCoordinator.getSpecificSystem<LogicSystemECS>();
	// Initialize ordered components that should always be present first

	int entityLayer = layerManager.getLayerFromEntity(entity);

	nlohmann::ordered_json entityJSON = {
		{"id", entityId},
		{"textureId", textureId},
		{"layer", entityLayer},
		{"transform", {
			{"localTransform", {
				{transform.mdl_xform.GetMatrixValue(0, 0), transform.mdl_xform.GetMatrixValue(0, 1), transform.mdl_xform.GetMatrixValue(0, 2)},
				{transform.mdl_xform.GetMatrixValue(1, 0), transform.mdl_xform.GetMatrixValue(1, 1), transform.mdl_xform.GetMatrixValue(1, 2)},
				{transform.mdl_xform.GetMatrixValue(2, 0), transform.mdl_xform.GetMatrixValue(2, 1), transform.mdl_xform.GetMatrixValue(2, 2)}
			}},
			{"orientation", {
				{"x", transform.orientation.GetX()},
				{"y", transform.orientation.GetY()}
			}},
			{"position", {
				{"x", transform.position.GetX()},
				{"y", transform.position.GetY()}
			}},
			{"projectionMatrix", {
				{transform.mdl_to_ndc_xform.GetMatrixValue(0, 0), transform.mdl_to_ndc_xform.GetMatrixValue(0, 1), transform.mdl_to_ndc_xform.GetMatrixValue(0, 2)},
				{transform.mdl_to_ndc_xform.GetMatrixValue(1, 0), transform.mdl_to_ndc_xform.GetMatrixValue(1, 1), transform.mdl_to_ndc_xform.GetMatrixValue(1, 2)},
				{transform.mdl_to_ndc_xform.GetMatrixValue(2, 0), transform.mdl_to_ndc_xform.GetMatrixValue(2, 1), transform.mdl_to_ndc_xform.GetMatrixValue(2, 2)}
			}},
			{"scale", {
				{"x", transform.scale.GetX()},
				{"y", transform.scale.GetY()}
			}}
		}}
	};

	if (ecs.hasComponent<BackgroundComponent>(entity)) {
		auto& background = ecs.getComponent<BackgroundComponent>(entity);

		background.isBackground = true;
		entityJSON["background"] = { {"isBackground", background.isBackground} };

	}

	// Add additional components in the desired order
	if (ecs.hasComponent<AABBComponent>(entity)) {
		auto& aabb = ecs.getComponent<AABBComponent>(entity);
		entityJSON["aabb"] = {
			{"bottom", aabb.bottom},
			{"left", aabb.left},
			{"right", aabb.right},
			{"top", aabb.top}
		};
	}

	if (ecs.hasComponent<PhysicsComponent>(entity)) {
		auto& rb = ecs.getComponent<PhysicsComponent>(entity);
		entityJSON["forces"] = {
			{"mass", rb.mass},
			{"gravityScale", {
				{"x", rb.gravityScale.GetX()},
				{"y", rb.gravityScale.GetY()}
			}},
			{"jump", rb.jump},
			{"dampening", rb.dampening},
			{"velocity", {
				{"x", rb.velocity.GetX()},
				{"y", rb.velocity.GetY()}
			}},
			{"maxVelocity", rb.maxVelocity},
			{"acceleration", {
				{"x", rb.acceleration.GetX()},
				{"y", rb.acceleration.GetY()}
			}},
			{"force", {
				{"direction", {
					{"x", rb.force.GetDirection().GetX()},
					{"y", rb.force.GetDirection().GetY()}
				}},
				{"magnitude", rb.force.GetMagnitude()}
			}},
			{"accumulatedForce", {
				{"x", rb.accumulatedForce.GetX()},
				{"y", rb.accumulatedForce.GetY()}
			}},
			{"maxAccumulatedForce", rb.maxAccumulatedForce},
			{"prevForce", rb.prevForce},
			{"targetForce", rb.targetForce}
		};
	}

	if (ecs.hasComponent<EnemyComponent>(entity)) {
		entityJSON["enemy"] = { {"isEnemy", true} };
	}

	if (ecs.hasComponent<ClosestPlatform>(entity)) {
		auto& platform = ecs.getComponent<ClosestPlatform>(entity);
		entityJSON["closestPlatform"] = { {"isClosest", platform.isClosest} };
	}

	if (ecs.hasComponent<AnimationComponent>(entity)) {
		auto& animation = ecs.getComponent<AnimationComponent>(entity);
		entityJSON["animation"] = {
			{"isAnimated", animation.isAnimated},
			{"totalFrames", animation.totalFrames},
			{"frameTime", animation.frameTime},
			{"columns", animation.columns},
			{"rows", animation.rows}
		};
	}

	if (ecs.hasComponent<FontComponent>(entity)) {
		auto& fontComp = ecs.getComponent<FontComponent>(entity);
		entityJSON["font"] = nlohmann::ordered_json{
			{"text", {
				{"string", fontComp.text},
				{"BoxWidth", fontComp.textBoxWidth}
			}},
			{"textScale", {
				{"scale", fontComp.textScale}
			}},
			{"color", {
				{"x", fontComp.color.GetX()},
				{"y", fontComp.color.GetY()},
				{"z", fontComp.color.GetZ()}
			}},
			{"fontId", {
				{"fontName", fontComp.fontId}
			}}
		};
	}

	if (ecs.hasComponent<PlayerComponent>(entity)) {
		auto& player = ecs.getComponent<PlayerComponent>(entity);
		player.isPlayer = true;
		entityJSON["player"] = { { "isPlayer", player.isPlayer } };
	}

	if (ecs.hasComponent<PumpComponent>(entity)) {
		auto& pump = ecs.getComponent<PumpComponent>(entity);
		pump.isPump = true;
		pump.pumpForce = 3.0f;
		entityJSON["pump"] = {
			{"isPump", pump.isPump},
			{"pumpForce", pump.pumpForce},
			{"isAnimate", pump.isAnimate }
		};
	}

	if (ecs.hasComponent<ExitComponent>(entity)) {
		auto& exit = ecs.getComponent<ExitComponent>(entity);
		exit.isExit = true;
		entityJSON["exit"] = { {"isExit", exit.isExit} };
	}

	if (ecs.hasComponent<ButtonComponent>(entity)) {
		auto& button = ecs.getComponent<ButtonComponent>(entity);
		button.isButton = true;
		entityJSON["button"] = nlohmann::ordered_json{
			{"hoveredScale", {
				{"x", button.hoveredScale.GetX()},
				{"y", button.hoveredScale.GetY()}
			}},
			{"isButton", button.isButton}
		};
	}

	if (ecs.hasComponent<CollectableComponent>(entity)) {
		auto& collectable = ecs.getComponent<CollectableComponent>(entity);
		collectable.isCollectable = true;
		entityJSON["collectable"] = { {"isCollectable", collectable.isCollectable} };
	}

	if (ecs.hasComponent<UIComponent>(entity)) {
		auto& UI = ecs.getComponent<UIComponent>(entity);
		UI.isUI = true;
		entityJSON["UI"] = { {"isUI", UI.isUI} };
	}

	if (ecs.hasComponent<FilterComponent>(entity)) {
		auto& filter = ecs.getComponent<FilterComponent>(entity);
		filter.isFilter = true;
		filter.isFilterClogged = false;
		entityJSON["filter"] = {
			{"isFilter", filter.isFilter},
			{"isFilterClogged", filter.isFilterClogged}
		};
	}

	//std::cout << "Has Behaviour:" << ecs.hasComponent<BehaviourComponent>(entity) << std::endl;
	if (ecs.hasComponent<BehaviourComponent>(entity)) {
		auto& behaviour = ecs.getComponent<BehaviourComponent>(entity);

		if (!logicSystemRef->hasBehaviour(entity)) {
			behaviour.none = true;
			entityJSON["behaviour"] = { {"none", behaviour.none} };
		}
		else if (logicSystemRef->hasBehaviour<PlayerBehaviour>(entity)) {
			behaviour.player = true;
			entityJSON["behaviour"] = { {"player", behaviour.player} };
		}
		else if (logicSystemRef->hasBehaviour<EnemyBehaviour>(entity)) {
			behaviour.enemy = true;
			entityJSON["behaviour"] = { {"enemy", behaviour.enemy} };
		}
		else if (logicSystemRef->hasBehaviour<EffectPumpBehaviour>(entity)) {
			behaviour.pump = true;
			entityJSON["behaviour"] = { {"pump", behaviour.pump} };
		}
		else if (logicSystemRef->hasBehaviour<ExitBehaviour>(entity)) {
			behaviour.exit = true;
			entityJSON["behaviour"] = { {"exit", behaviour.exit} };
		}
		else if (logicSystemRef->hasBehaviour<CollectableBehaviour>(entity)) {
			behaviour.collectable = true;
			entityJSON["behaviour"] = { {"collectable", behaviour.collectable} };
		}
		else if (logicSystemRef->hasBehaviour<MouseBehaviour>(entity)) {
			behaviour.button = true;
			entityJSON["behaviour"] = { {"button", behaviour.button} };
		}
		else if (logicSystemRef->hasBehaviour<PlatformBehaviour>(entity)) {
			behaviour.platform = true;
			entityJSON["behaviour"] = { {"platform", behaviour.platform} };
		}
		else if (logicSystemRef->hasBehaviour<FilterBehaviour>(entity)) {
			behaviour.filter = true;
			entityJSON["behaviour"] = { {"filter", behaviour.filter} };
		}
		else if (logicSystemRef->hasBehaviour<MovPlatformBehaviour>(entity)) {
			behaviour.movPlatform = true;
			entityJSON["behaviour"] = { {"movPlatform", behaviour.movPlatform} };
		}
	}

	return entityJSON;
}

ImVec2 GameViewWindow::GetCenteredMousePosition() {

	// Calculate the game scene bounds in screen space
	float gameLeft = viewportPos.x + renderPos.x;
	float gameTop = viewportPos.y + renderPos.y;

	// Get mouse position relative to game scene origin
	float sceneX = globalMousePos.x - (gameLeft + aspectSize.x * 0.5f);
	float sceneY = (gameTop + aspectSize.y * 0.5f) - globalMousePos.y;

	// Check if mouse is within game scene bounds
	if (globalMousePos.x < gameLeft || globalMousePos.x > gameLeft + aspectSize.x ||
		globalMousePos.y < gameTop || globalMousePos.y > gameTop + aspectSize.y) {
		return ImVec2(0, 0);
	}

	// Map the coordinates to ?800 x ?450 range
	float normalizedX = (sceneX / aspectSize.x) * 800.0f * 2.0f;
	float normalizedY = (sceneY / aspectSize.y) * 450.0f * 2.0f;

	return ImVec2(normalizedX, normalizedY);
}

ImVec2 GameViewWindow::NormalizeViewportCoordinates(float screenX, float screenY) {
	myMath::Vector2D cameraPos = cameraSystem.getCameraPosition();

	// Get rotation in radians, normalize it to keep it between 0 and 2PI
	double playerRotation = 0.0;
	for (auto entity : ecsCoordinator.getAllLiveEntities()) {
		if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
			playerRotation = ecsCoordinator.getComponent<TransformComponent>(entity).orientation.GetX();
			break;
		}
	}
	// Convert to radians and normalize
	double cameraRotation = fmod(playerRotation * (M_PI / 180.0f), 2.0f * M_PI);
	if (cameraRotation < 0) cameraRotation += 2.0f * M_PI;

	// Get viewport center in screen space
	double viewportCenterX = viewportPos.x + renderPos.x + (aspectSize.x * 0.5f);
	double viewportCenterY = viewportPos.y + renderPos.y + (aspectSize.y * 0.5f);

	// Convert screen coordinates to viewport-relative coordinates
	double viewportX = screenX - viewportCenterX;
	double viewportY = viewportCenterY - screenY;

	// Scale by zoom and viewport size
	double worldScaleX = (viewportWidth / (aspectSize.x * currentZoom));
	double worldScaleY = (viewportHeight / (aspectSize.y * currentZoom));

	viewportX *= worldScaleX;
	viewportY *= worldScaleY;

	// Calculate rotation factors
	double cosAngle = cos(cameraRotation);
	double sinAngle = sin(cameraRotation);

	// Rotate coordinates
	double rotatedX = viewportX * cosAngle - viewportY * sinAngle;
	double rotatedY = viewportX * sinAngle + viewportY * cosAngle;

	// Transform to world space
	double worldX = cameraPos.GetX() + rotatedX;
	double worldY = cameraPos.GetY() + rotatedY;

	// Apply pan offset
	//if (clickedScreenPan) {
	double panX = accumulatedMouseDragDist.x / (currentZoom * GLFWFunctions::windowWidth);
	double panY = accumulatedMouseDragDist.y / (currentZoom * GLFWFunctions::windowHeight);

	worldX += panX * cosAngle - panY * sinAngle;
	worldY += panX * sinAngle + panY * cosAngle;
	//}

	return ImVec2(static_cast<float>(worldX), static_cast<float>(worldY));
}

ImVec2 GameViewWindow::GetMouseWorldPosition() {
	if (IsPointInViewport(globalMousePos.x, globalMousePos.y)) {
		return NormalizeViewportCoordinates(globalMousePos.x, globalMousePos.y);
	}
	return ImVec2(0.0f, 0.0f);
}
// Convert screen coordinates to viewport coordinates
ImVec2 GameViewWindow::ScreenToViewport(float screenX, float screenY) {

	return ImVec2(
		screenX - (viewportPos.x + renderPos.x),
		screenY - (viewportPos.y + renderPos.y)
	);
}

// Convert viewport coordinates to game world coordinates
ImVec2 GameViewWindow::ViewportToWorld(float viewportX, float viewportY) {
	myMath::Vector2D cameraPos = cameraSystem.getCameraPosition();

	// Get rotation in radians, normalize it to keep it between 0 and 2PI
	double playerRotation = 0.0;
	for (auto entity : ecsCoordinator.getAllLiveEntities()) {
		if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
			playerRotation = ecsCoordinator.getComponent<TransformComponent>(entity).orientation.GetX();
			break;
		}
	}
	// Convert to radians and normalize
	double cameraRotation = fmod(playerRotation * (M_PI / 180.0f), 2.0f * M_PI);
	if (cameraRotation < 0) cameraRotation += 2.0f * M_PI;

	// Get viewport center
	float viewportCenterX = aspectSize.x * 0.5f;
	float viewportCenterY = aspectSize.y * 0.5f;

	// Calculate relative coordinates
	float relX = viewportX - viewportCenterX;
	float relY = viewportCenterY - viewportY;

	// Scale to world units
	float worldScaleX = (viewportWidth / (aspectSize.x * currentZoom));
	float worldScaleY = (viewportHeight / (aspectSize.y * currentZoom));

	relX *= worldScaleX;
	relY *= worldScaleY;

	// Calculate rotation factors
	double cosAngle = cos(cameraRotation);
	double sinAngle = sin(cameraRotation);

	// Rotate coordinates
	double rotatedX = relX * cosAngle - relY * sinAngle;
	double rotatedY = relX * sinAngle + relY * cosAngle;

	// Transform to world space
	double worldX = cameraPos.GetX() + rotatedX;
	double worldY = cameraPos.GetY() + rotatedY;

	// Apply pan offset
	//if (clickedScreenPan) {
	float panX = accumulatedMouseDragDist.x / (currentZoom * GLFWFunctions::windowWidth);
	float panY = accumulatedMouseDragDist.y / (currentZoom * GLFWFunctions::windowHeight);

	worldX += panX * cosAngle - panY * sinAngle;
	worldY += panX * sinAngle + panY * cosAngle;
	//}

	return ImVec2(static_cast<float>(worldX), static_cast<float>(worldY));
}

ImVec2 GameViewWindow::ScreenToWorld(float screenX, float screenY) {
	ImVec2 viewportCoords = ScreenToViewport(screenX, screenY);
	return ViewportToWorld(viewportCoords.x, viewportCoords.y);
}

// Load the IMGUI viewport configuration values from JSON file
void GameViewWindow::LoadViewportConfigFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	std::cout << filename << std::endl;

	// checks if JSON file can be opened
	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	// return the entire JSON object from the file
	nlohmann::json jsonObj = serializer.GetJSONObject();

	// read all of the data from the JSON object and assign all the read data to
	// data elements of GameViewWindow class that needs to be initialized
	serializer.ReadInt(viewportWidth, "GUIViewport.viewportWidth");
	serializer.ReadInt(viewportHeight, "GUIViewport.viewportHeight");

	serializer.ReadInt(saveNum, "GUIViewport.saveNum");
	serializer.ReadInt(fileNum, "GUIViewport.fileNum");

	serializer.ReadFloat(viewportPos.x, "GUIViewport.viewportPos.x");
	serializer.ReadFloat(viewportPos.y, "GUIViewport.viewportPos.y");

	serializer.ReadUnsignedInt(viewportTexture, "GUIViewport.viewportTexture");

	serializer.ReadFloat(zoomLevel, "GUIViewport.zoomLevel");

	serializer.ReadFloat(MIN_ZOOM, "GUIViewport.minZoom");
	serializer.ReadFloat(MAX_ZOOM, "GUIViewport.maxZoom");

	serializer.ReadFloat(scrollY, "GUIViewport.scrollY");

	serializer.ReadFloat(globalMousePos.x, "GUIViewport.currentMousePosX");
	serializer.ReadFloat(globalMousePos.y, "GUIViewport.currentMousePosY");

	serializer.ReadFloat(accumulatedMouseDragDist.x, "GUIViewport.accumulatedMouseDragDist.x");
	serializer.ReadFloat(accumulatedMouseDragDist.y, "GUIViewport.accumulatedMouseDragDist.y");

	serializer.ReadFloat(initialMousePos.x, "GUIViewport.initialMousePos.x");
	serializer.ReadFloat(initialMousePos.y, "GUIViewport.initialMousePos.y");

	serializer.ReadFloat(mouseDragDist.x, "GUIViewport.mouseDragDist.x");
	serializer.ReadFloat(mouseDragDist.y, "GUIViewport.mouseDragDist.y");
	serializer.ReadFloat(mouseDragSpeed, "GUIViewport.mouseDragSpeed");

	serializer.ReadFloat(aspectRatioWidth, "GUIViewport.aspectRatioWidth");
	serializer.ReadFloat(aspectRatioHeight, "GUIViewport.aspectRatioHeight");

	serializer.ReadFloat(lastPosX, "GUIViewport.lastPosX");
	serializer.ReadFloat(saveWindowWidth, "GUIViewport.saveWindowWidth");
	serializer.ReadFloat(fileWindowWidth, "GUIViewport.fileWindowWidth");
	serializer.ReadFloat(saveWindowHeight, "GUIViewport.saveWindowHeight");
	serializer.ReadFloat(fileWindowHeight, "GUIViewport.fileWindowHeight");
	serializer.ReadFloat(slotWindowWidth, "GUIViewport.slotWindowWidth");
	serializer.ReadFloat(clearSlotWindowWidth, "GUIViewport.clearSlotWindowWidth");
	serializer.ReadFloat(optionsButtonPadding, "GUIViewport.optionsButtonPadding");

	serializer.ReadFloat(mouseWheelScaleFactor, "GUIViewport.mouseWheelScaleFactor");
	serializer.ReadFloat(initialZoom, "GUIViewport.initialZoom");
	serializer.ReadInt(saveLimit, "GUIViewport.saveLimit");

}

// load the scene number from the scene JSON file
void GameViewWindow::LoadSceneFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json currentObj = serializer.GetJSONObject();

	serializer.ReadInt(scene, "scene");
}

// save the updated scene number to the scene JSON file
void GameViewWindow::SaveSceneToJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();

	serializer.WriteInt(scene, "scene", filename);
}

std::string GameViewWindow::GetPrefabJSONPath(std::string const& filename)
{
	std::string execPath = FilePathManager::GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\Sandbox\\assets\\prefabs\\" + filename;
	return jsonPath;
}

void GameViewWindow::LoadPrefabFromJSON(std::string const& filename, std::string const& filepath)
{
	JSONSerializer serializer;

	//extract out last part of the file path
	std::string assetPathName = filepath.substr(filepath.find_last_of("\\/") + 1);
    assetPathName = GetPrefabJSONPath(assetPathName);

	if (!serializer.Open(assetPathName))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}
	nlohmann::json currentObj = serializer.GetJSONObject();

	auto logicSystemRef = ecsCoordinator.getSpecificSystem<LogicSystemECS>();

	Entity prefabEntity = ecsCoordinator.createEntity();
	//get entity ID and texture ID
	std::string entityId = currentObj["id"];
	entityId += std::to_string(objectCounter); 
	std::string textureId = currentObj["textureId"];

	ecsCoordinator.setEntityID(prefabEntity, entityId);
	ecsCoordinator.setTextureID(prefabEntity, textureId);

	//default add to layer 0
	layerManager.addEntityToLayer(0, prefabEntity);

	TransformComponent transform;
	serializer.ReadObject(transform.mdl_xform, filepath, "transform.localTransform");
	serializer.ReadObject(transform.orientation, filepath, "transform.orientation");
	serializer.ReadObject(transform.position, filepath, "transform.position");
	serializer.ReadObject(transform.mdl_to_ndc_xform, filepath, "transform.projectionMatrix");
	serializer.ReadObject(transform.scale, filepath, "transform.scale");

	ecsCoordinator.addComponent(prefabEntity, transform);

	// Add the appropriate components based on the specifier
	if (currentObj.contains("aabb")) {
		AABBComponent aabb;
		serializer.ReadObject(aabb.bottom, filepath, "aabb.bottom");
		serializer.ReadObject(aabb.left, filepath, "aabb.left");
		serializer.ReadObject(aabb.right, filepath, "aabb.right");
		serializer.ReadObject(aabb.top, filepath, "aabb.top");

		ecsCoordinator.addComponent(prefabEntity, aabb);
	}

	if (currentObj.contains("forces")) {
		PhysicsComponent forces{};

		myMath::Vector2D direction = forces.force.GetDirection();
		float magnitude = forces.force.GetMagnitude();

		serializer.ReadObject(forces.mass, entityId, "forces.mass");
		serializer.ReadObject(forces.gravityScale, entityId, "forces.gravityScale");
		serializer.ReadObject(forces.jump, entityId, "forces.jump");
		serializer.ReadObject(forces.dampening, entityId, "forces.dampening");
		serializer.ReadObject(forces.velocity, entityId, "forces.velocity");
		serializer.ReadObject(forces.maxVelocity, entityId, "forces.maxVelocity");
		serializer.ReadObject(forces.acceleration, entityId, "forces.acceleration");
		serializer.ReadObject(direction, entityId, "forces.force.direction");
		serializer.ReadObject(magnitude, entityId, "forces.force.magnitude");
		serializer.ReadObject(forces.accumulatedForce, entityId, "forces.accumulatedForce");
		serializer.ReadObject(forces.maxAccumulatedForce, entityId, "forces.maxAccumulatedForce");
		serializer.ReadObject(forces.prevForce, entityId, "forces.prevForce");
		serializer.ReadObject(forces.targetForce, entityId, "forces.targetForce");

		forces.force.SetDirection(direction);
		forces.force.SetMagnitude(magnitude);

		ecsCoordinator.addComponent(prefabEntity, forces);
	}

	if (currentObj.contains("player")) {
		PlayerComponent player;
		serializer.ReadObject(player.isPlayer, entityId, " player.isPlayer");

		ecsCoordinator.addComponent(prefabEntity, player);
	}

	if (currentObj.contains("behaviour")) {
		BehaviourComponent behaviour{};
		if (currentObj["behaviour"].contains("none")) {
			serializer.ReadObject(behaviour.none, entityId, "behaviour.none");
			logicSystemRef->unassignBehaviour(prefabEntity);
		}
		else if (currentObj["behaviour"].contains("player")) {
			serializer.ReadObject(behaviour.player, entityId, "behaviour.player");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<PlayerBehaviour>());
		}
		else if (currentObj["behaviour"].contains("enemy")) {
			serializer.ReadObject(behaviour.enemy, entityId, "behaviour.enemy");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<EnemyBehaviour>());
		}
		else if (currentObj["behaviour"].contains("pump")) {
			serializer.ReadObject(behaviour.pump, entityId, "behaviour.pump");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<EffectPumpBehaviour>());
		}
		else if (currentObj["behaviour"].contains("exit")) {
			serializer.ReadObject(behaviour.exit, entityId, "behaviour.exit");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<ExitBehaviour>());
		}
		else if (currentObj["behaviour"].contains("collectable")) {
			serializer.ReadObject(behaviour.collectable, entityId, "behaviour.collectable");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<CollectableBehaviour>());
		}
		else if (currentObj["behaviour"].contains("button")) {
			serializer.ReadObject(behaviour.button, entityId, "behaviour.button");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<MouseBehaviour>());
		}
		else if (currentObj["behaviour"].contains("platform")) {
			serializer.ReadObject(behaviour.platform, entityId, "behaviour.platform");
			//logicSystemRef->assignBehaviour(entityObj, std::make_shared<PlatformBehaviour>());
		}
		else if (currentObj["behaviour"].contains("filter")) {
			serializer.ReadObject(behaviour.platform, entityId, "behaviour.filter");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<FilterBehaviour>());
		}
		else if (currentObj["behaviour"].contains("movPlatform")) {
			serializer.ReadObject(behaviour.platform, entityId, "behaviour.movPlatform");
			logicSystemRef->assignBehaviour(prefabEntity, std::make_shared<MovPlatformBehaviour>());
		}

		ecsCoordinator.addComponent(prefabEntity, behaviour);
	}

	if (currentObj.contains("animation")) {
		AnimationComponent animation;
		serializer.ReadObject(animation.isAnimated, entityId, "animation.isAnimated");
		serializer.ReadObject(animation.totalFrames, entityId, "animation.totalFrames");
		serializer.ReadObject(animation.frameTime, entityId, "animation.frameTime");
		serializer.ReadObject(animation.columns, entityId, "animation.columns");
		serializer.ReadObject(animation.rows, entityId, "animation.rows");

		ecsCoordinator.addComponent(prefabEntity, animation);
	}

	if (currentObj.contains("collectable")) {
		CollectableComponent collectable;
		serializer.ReadObject(collectable.isCollectable, entityId, "collectable.isCollectable");
		ecsCoordinator.addComponent(prefabEntity, collectable);
	}

	if (currentObj.contains("pump")) {
		PumpComponent pump;
		serializer.ReadObject(pump.isPump, entityId, "pump.isPump");
		serializer.ReadObject(pump.pumpForce, entityId, "pump.pumpForce");
		serializer.ReadObject(pump.isAnimate, entityId, "pump.isAnimate");

		ecsCoordinator.addComponent(prefabEntity, pump);
	}

	if (currentObj.contains("filter")) {
		FilterComponent filter;
		serializer.ReadObject(filter.isFilter, entityId, "filter.isFilter");
		serializer.ReadObject(filter.isFilterClogged, entityId, "filter.isFilterClogged");
		ecsCoordinator.addComponent(prefabEntity, filter);
	}

	if (currentObj.contains("enemy")) {
		EnemyComponent enemy;
		serializer.ReadObject(enemy.isEnemy, entityId, "enemy.isEnemy");
		ecsCoordinator.addComponent(prefabEntity, enemy);
	}
}