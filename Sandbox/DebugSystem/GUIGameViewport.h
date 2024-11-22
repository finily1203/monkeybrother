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
#include "GlobalCoordinator.h"

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
		// Get viewport dimensions and position
		ImVec2 aspectSize = GetLargestSizeForViewport();
		ImVec2 viewPos = GetCenteredPosForViewport(aspectSize);

		myMath::Vector2D referencePos;
		if (!GLFWFunctions::allow_camera_movement) {
			// Camera is locked to player
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				if (ecsCoordinator.getEntityID(entity) == "player") {
					auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					referencePos = transform.position;
					break;
				}
			}
		}
		else {
			// Free camera movement - use global cameraSystem
			referencePos = cameraSystem.getCameraPosition();
		}

		// Calculate viewport-local coordinates
		float viewX = screenX - (viewportPos.x + viewPos.x);
		float viewY = screenY - (viewportPos.y + viewPos.y);

		// Convert to normalized device coordinates (-1 to 1) relative to viewport center
		float ndcX = (viewX / aspectSize.x) * 2.0f - 1.0f;
		float ndcY = -((viewY / aspectSize.y) * 2.0f - 1.0f); // Flip Y-axis

		// Convert to world coordinates relative to player position
		float worldX = referencePos.GetX() + ((ndcX * viewportWidth) / (2.0f * zoomLevel));
		float worldY = referencePos.GetY() + ((ndcY * viewportHeight) / (2.0f * zoomLevel));

		// Apply pan offset
		worldX += accumulatedMouseDragDist.x / (zoomLevel * aspectRatioXScale);
		worldY -= accumulatedMouseDragDist.y / (zoomLevel * aspectRatioYScale);

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
		ImVec2 aspectSize = GetLargestSizeForViewport();
		ImVec2 viewPos = GetCenteredPosForViewport(aspectSize);

		return ImVec2(
			screenX - (viewportPos.x + viewPos.x),
			screenY - (viewportPos.y + viewPos.y)
		);
	}

	// Convert viewport coordinates to game world coordinates
	static ImVec2 ViewportToWorld(float viewportX, float viewportY) {
		ImVec2 size = GetLargestSizeForViewport();

		myMath::Vector2D referencePos;
		if (!GLFWFunctions::allow_camera_movement) {
			// Camera is locked to player
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				if (ecsCoordinator.getEntityID(entity) == "player") {
					auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					referencePos = transform.position;
					break;
				}
			}
		}
		else {
			// Free camera movement - use global cameraSystem
			referencePos = cameraSystem.getCameraPosition();
		}

		// Calculate normalized coordinates relative to viewport center
		float normalizedX = (viewportX / size.x) * 2.0f - 1.0f;
		float normalizedY = -((viewportY / size.y) * 2.0f - 1.0f); // Flip Y-axis

		// Convert to world coordinates relative to player position
		float worldX = referencePos.GetX() + ((normalizedX * viewportWidth) / (2.0f * zoomLevel));
		float worldY = referencePos.GetY() + ((normalizedY * viewportHeight) / (2.0f * zoomLevel));

		// Apply pan offset
		worldX += accumulatedMouseDragDist.x / (zoomLevel * aspectRatioXScale);
		worldY -= accumulatedMouseDragDist.y / (zoomLevel * aspectRatioYScale);

		return ImVec2(worldX, worldY);
	}

	// Convert screen coordinates directly to world coordinates
	static ImVec2 ScreenToWorld(float screenX, float screenY) {
		ImVec2 viewportCoords = ScreenToViewport(screenX, screenY);
		return ViewportToWorld(viewportCoords.x, viewportCoords.y);
	}

	// Helper method to check if world coordinates are in view
	static bool IsWorldPointVisible(float worldX, float worldY) {
		myMath::Vector2D referencePos;
		if (!GLFWFunctions::allow_camera_movement) {
			// Camera is locked to player
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				if (ecsCoordinator.getEntityID(entity) == "player") {
					auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					referencePos = transform.position;
					break;
				}
			}
		}
		else {
			// Free camera movement - use global cameraSystem
			referencePos = cameraSystem.getCameraPosition();
		}

		// Convert world bounds to viewport space considering zoom and pan
		float viewWidth = viewportWidth / zoomLevel;
		float viewHeight = viewportHeight / zoomLevel;

		// Calculate bounds relative to player position
		float minX = referencePos.GetX() - viewWidth / 2;
		float maxX = referencePos.GetX() + viewWidth / 2;
		float minY = referencePos.GetY() - viewHeight / 2;
		float maxY = referencePos.GetY() + viewHeight / 2;

		// Account for panning
		float panX = accumulatedMouseDragDist.x / (zoomLevel * aspectRatioXScale);
		float panY = accumulatedMouseDragDist.y / (zoomLevel * aspectRatioYScale);

		return worldX >= minX - panX &&
			worldX <= maxX - panX &&
			worldY >= minY + panY &&
			worldY <= maxY + panY;
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

	static bool getPaused() { return isPaused; }

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