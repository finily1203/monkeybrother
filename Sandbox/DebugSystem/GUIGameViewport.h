#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 

//Class for ImGui game viewport window in debugging mode
class GameViewWindow {
public:

	static float zoomLevel;
	static float MIN_ZOOM;  // minimum zoom constant
	static float MAX_ZOOM;  // maximum zoom constant

	static void Initialise();

	static void Update();

	static bool IsPointInViewport(double x, double y);

	static ImVec2 getViewportPos() { return viewportPos; }
	static void LoadViewportConfigFromJSON(std::string const& filename);

private:

	static void setAccumulatedDragDistance(float valueX, float valueY) {
		accumulatedDragDistanceX = valueX;
		accumulatedDragDistanceY = valueY;
	}

	static void setClickedZoom(bool state) {
		clickedZoom = state;
	}

	static void setClickedScreenPan(bool state) {
		clickedScreenPan = state;
	}

private:
	static int viewportWidth;
	static int viewportHeight;
	static ImVec2 viewportPos;
	static GLuint viewportTexture;
	static ImVec2 lastViewportSize;
	static ImVec2 lastAspectSize;
	static ImVec2 lastRenderPos;
	static float accumulatedDragDistanceX;
	static float accumulatedDragDistanceY;
	static bool clickedZoom;
	static bool clickedScreenPan;
	//static ImVec2 windowSize;

	static void SetupViewportTexture(); //Set up Opengl texture to store game scene

	static void CaptureMainWindow(); //Capture rendered game scene

	static void Cleanup();

	static ImVec2 GetLargestSizeForViewport(); //Resize viewport dynamically while preserving aspect ratio

	static ImVec2 GetCenteredPosForViewport(ImVec2 aspectSize); //Center viewport within available space

	static void UpdateViewportSize() {

		lastViewportSize = ImGui::GetContentRegionAvail(); // size of viewport
		lastAspectSize = GetLargestSizeForViewport();
		lastRenderPos = GetCenteredPosForViewport(lastAspectSize);

	}

};