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

File Contributions: Lew Zong Han Owen (90%)
					Ian Loi           (10%)

/*_______________________________________________________________________________________________________________*/
#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 

//enum for creating game obj
enum Specifier {
	TEXTURE,
	FONT,
};

//Class for ImGui game viewport window in debugging mode
class GameViewWindow {
public:

	static void Initialise();

	static void Update();

	static bool IsPointInViewport(double x, double y);

	static ImVec2 getViewportPos() { return viewportPos; }
	static void LoadViewportConfigFromJSON(std::string const& filename);
	static void SaveViewportConfigToJSON(std::string const& filename);

	static void createDropEntity(const char* assetName, Specifier specifier);

	static void setAccumulatedDragDistance(float valueX, float valueY) {
		accumulatedMouseDragDist.x = valueX;
		accumulatedMouseDragDist.y = valueY;
	}

	static void setZoomLevel(float value) {
		zoomLevel = value;
	}

	static void setClickedZoom(bool state) {
		clickedZoom = state;
	}

	static void setClickedScreenPan(bool state) {
		clickedScreenPan = state;
	}

	float Clamp(float value, float min, float max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

private:
	static int viewportWidth;
	static int viewportHeight;
	static ImVec2 viewportPos;
	static ImVec2 windowSize;
	static GLuint viewportTexture;
	static bool clickedZoom;
	static bool clickedScreenPan;

	static float newZoomLevel;
	static float zoomDelta;
	static float zoomLevel;
	static float MIN_ZOOM;  // minimum zoom constant
	static float MAX_ZOOM;  // maximum zoom constant

	static ImVec2 currentMousePos;
	static float scrollY;

	static bool isDragging;
	static ImVec2 accumulatedMouseDragDist;
	static ImVec2 currentMouseDragDist;
	static ImVec2 initialMousePos;
	static ImVec2 mouseDragDist;

	static float aspectRatioXScale;
	static float aspectRatioYScale;
	static float aspectRatioWidth;
	static float aspectRatioHeight;

	static void SetupViewportTexture(); //Set up Opengl texture to store game scene

	static void CaptureMainWindow(); //Capture rendered game scene

	static void Cleanup();

	static ImVec2 GetLargestSizeForViewport(); //Resize viewport dynamically while preserving aspect ratio

	static ImVec2 GetCenteredPosForViewport(ImVec2 aspectSize); //Center viewport within available space

};