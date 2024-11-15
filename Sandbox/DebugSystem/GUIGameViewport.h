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
	static void TogglePause() { isPaused = !isPaused; }
	static bool IsPaused() { return isPaused; }
	static ImVec2 NormalizeViewportCoordinates(float screenX, float screenY) {
		//myMath::Vector2D cameraPos = GlobalCoordinator::GetInstance().GetSystemManager().GetSystem<CameraSystem2D>()->getCameraPosition();
		// Get mouse position relative to viewport
		ImVec2 aspectSize = GetLargestSizeForViewport();
		float centerOffsetX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
		float centerOffsetY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

		// Adjust viewport position to account for centering
		float viewX = screenX - (viewportPos.x + centerOffsetX);
		float viewY = screenY - (viewportPos.y + centerOffsetY);

		// Convert to normalized device coordinates (-1 to 1)
		float ndcX = (2.0f * viewX / aspectSize.x) - 1.0f;
		float ndcY = 1.0f - (2.0f * viewY / aspectSize.y);

		// Scale by zoom and apply pan offset
		ndcX = (ndcX + accumulatedMouseDragDist.x / windowSize.x) / zoomLevel;
		ndcY = (ndcY + accumulatedMouseDragDist.y / windowSize.y) / zoomLevel;

		// Convert to world coordinates based on your viewport size
		float worldX = ndcX * (viewportWidth / 2.0f);
		float worldY = ndcY * (viewportHeight / 2.0f);

		return ImVec2(worldX, worldY);
	}

	// Update GetMouseWorldPosition():
	static ImVec2 GetMouseWorldPosition() {
		ImVec2 mousePos = ImGui::GetMousePos();
		if (IsPointInViewport(mousePos.x, mousePos.y)) {
			return NormalizeViewportCoordinates(mousePos.x, mousePos.y);
		}
		return ImVec2(0.0f, 0.0f);
	}
	// Convert screen coordinates to viewport coordinates
	static ImVec2 ScreenToViewport(float screenX, float screenY) {
		return ImVec2(
			screenX - viewportPos.x,
			screenY - viewportPos.y
		);
	}

	// Convert viewport coordinates to game world coordinates
	static ImVec2 ViewportToWorld(float viewportX, float viewportY) {
		// Get the current viewport size and scale
		ImVec2 size = GetLargestSizeForViewport();

		// Calculate the viewport center offset (including pan)
		float centerX = (windowSize.x / 2.0f) - (size.x / 2.0f) - accumulatedMouseDragDist.x;
		float centerY = (windowSize.y / 2.0f) - (size.y / 2.0f) - accumulatedMouseDragDist.y;

		// Convert to normalized coordinates (-1 to 1)
		float normalizedX = ((viewportX - centerX) / size.x) * 2.0f;
		float normalizedY = ((viewportY - centerY) / size.y) * 2.0f;

		// Apply zoom and aspect ratio
		float worldX = normalizedX * (aspectRatioXScale / zoomLevel);
		float worldY = normalizedY * (aspectRatioYScale / zoomLevel);

		return ImVec2(worldX, worldY);
	}

	// Convert screen coordinates directly to world coordinates
	static ImVec2 ScreenToWorld(float screenX, float screenY) {
		ImVec2 viewportCoords = ScreenToViewport(screenX, screenY);
		return ViewportToWorld(viewportCoords.x, viewportCoords.y);
	}

	// Helper method to check if world coordinates are in view
	static bool IsWorldPointVisible(float worldX, float worldY) {
		float viewWidth = aspectRatioXScale / zoomLevel;
		float viewHeight = aspectRatioYScale / zoomLevel;

		// Account for panning
		float panOffsetX = accumulatedMouseDragDist.x / (windowSize.x * zoomLevel);
		float panOffsetY = accumulatedMouseDragDist.y / (windowSize.y * zoomLevel);

		return worldX >= -viewWidth / 2 - panOffsetX &&
			worldX <= viewWidth / 2 - panOffsetX &&
			worldY >= -viewHeight / 2 - panOffsetY &&
			worldY <= viewHeight / 2 - panOffsetY;
	}

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

	static GLuint pausedTexture;
	static bool isPaused;

};