/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIGameViewport.h
@brief  :  This file contains the function declaration of ImGui game viewport system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui game viewport window to capture game scene in real time during debug mode
		- Integrated game viewport camera controls for zooming and panning current game scene in real time

*Ian Loi (ian.loi) :
		- Integrated serialization & deserialization functions to initialize variables from json file, which
		  allows saving and loading feature in the level editor

*Liu YaoTing (yaoting.liu) :
		- Integrated save and load function for multiple files
		- Updated "Pause", "Zoom", and "Pan" buttons

File Contributions: Lew Zong Han Owen (60%)
					Ian Loi           (10%)
					Liu YaoTing       (30%)

/*_______________________________________________________________________________________________________________*/
#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "GlobalCoordinator.h"

//enum for creating game obj
enum Specifier {
	TEXTURE,
	FONT,
};

//Class for ImGui game viewport window in debugging mode
class GameViewWindow {
public:
	static ImVec2 GetCenteredMousePosition();

	static void TogglePause() { isPaused = !isPaused;}
	static bool IsPaused() { return isPaused; }

	static ImVec2 NormalizeViewportCoordinates(float screenX, float screenY);

	// Update GetMouseWorldPosition():
	static ImVec2 GetMouseWorldPosition();

	static ImVec2 ScreenToViewport(float screenX, float screenY);

	// Convert viewport coordinates to game world coordinates
	static ImVec2 ViewportToWorld(float viewportX, float viewportY);

	// Convert screen coordinates directly to world coordinates
	static ImVec2 ScreenToWorld(float screenX, float screenY);

	static void Initialise();

	static void Update();

	static void Cleanup();

	static bool IsPointInViewport(double x, double y);

	static ImVec2 getViewportPos() { return viewportPos; }

	static void LoadViewportConfigFromJSON(std::string const& filename);
	static void SaveViewportConfigToJSON(std::string const& filename);

	static void LoadSceneFromJSON(std::string const& filename);
	static void SaveSceneToJSON(std::string const& filename);

	static void createDropEntity(const char* assetName, Specifier specifier);

	static void setAccumulatedDragDistance(float valueX, float valueY) {
		accumulatedMouseDragDist.x = valueX;
		accumulatedMouseDragDist.y = valueY;
	}

	static void setClickedZoom(bool state) {
		clickedZoom = state;
	}

	static void setClickedScreenPan(bool state) {
		clickedScreenPan = state;
	}

	static bool getPaused() { return isPaused; }

	static int getSceneNum() { return scene; }

	static void setSceneNum(int sceneNum) { scene = sceneNum; }

	static std::string GenerateSaveJSONFile(int& saveNumber);

	static nlohmann::ordered_json AddNewEntityToJSON(TransformComponent& transform, std::string const& entityId, std::string const& textureId, ECSCoordinator& ecs, Entity& entity);

	static ImVec2 GetLargestSizeForViewport(); //Resize viewport dynamically while preserving aspect ratio

	static ImVec2 GetCenteredPosForViewport(ImVec2 size); //Center viewport within available space

	static void SetupViewportTexture(); //Set up Opengl texture to store game scene

	static void CaptureMainWindow(); //Capture rendered game scene

private:
	static int saveNum;
	static int fileNum;
	static int viewportWidth;
	static int viewportHeight;
	static ImVec2 viewportPos;
	static ImVec2 windowSize;
	static GLuint viewportTexture;
	static bool clickedZoom;
	static bool clickedScreenPan;
	static ImVec2 aspectSize;
	static ImVec2 globalMousePos;
	static ImVec2 applicationCenter;
	static ImVec2 renderPos;

	static float currentZoom;
	static float zoomLevel;
	static float MIN_ZOOM;  // minimum zoom constant
	static float MAX_ZOOM;  // maximum zoom constant

	static float headerHeight;
	static float optionButtonHeight;
	static float scrollY;

	static bool isDragging;
	static ImVec2 accumulatedMouseDragDist;
	static ImVec2 currentMouseDragDist;
	static ImVec2 initialMousePos;
	static ImVec2 mouseDragDist;
	static float mouseDragSpeed;

	static float aspectRatioXScale;
	static float aspectRatioYScale;
	static float aspectRatioWidth;
	static float aspectRatioHeight;

	static bool isPaused;

	static int saveCount;
	static int saveLimit;
	static float lastPosX;
	static float saveWindowWidth;
	static float fileWindowWidth;
	static float saveWindowHeight;
	static float fileWindowHeight;
	static float slotWindowWidth;
	static float clearSlotWindowWidth;
	static float optionsButtonPadding;
	static float initialZoom;
	static float mouseWheelScaleFactor;

	static bool isSelectingSaveFile;
	static bool isSelectingFile;
	static bool saveFileChosen;
	static bool loadFileChosen;
	static int scene;
	static int objectCounter;

};