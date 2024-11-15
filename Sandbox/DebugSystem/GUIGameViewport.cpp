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

File Contributions: Lew Zong Han Owen (90%)
					Ian Loi           (10%)

/*_______________________________________________________________________________________________________________*/
#include "GUIGameViewport.h"
#include "GlfwFunctions.h"
#include "GUIConsole.h"
#include "Debug.h"

//Variables for GameViewWindow
int GameViewWindow::viewportHeight;
int GameViewWindow::viewportWidth;
ImVec2 GameViewWindow::viewportPos;
GLuint GameViewWindow::viewportTexture;
ImVec2 GameViewWindow::windowSize;

bool insideViewport = false;
bool zoomTestFlag = false;
bool GameViewWindow::clickedZoom = false;
float GameViewWindow::zoomLevel;
float GameViewWindow::newZoomLevel;
float GameViewWindow::zoomDelta;
float GameViewWindow::MIN_ZOOM;
float GameViewWindow::MAX_ZOOM;

ImVec2 GameViewWindow::currentMousePos;
float GameViewWindow::scrollY;

bool GameViewWindow::clickedScreenPan = false;
bool GameViewWindow::isDragging = false;
ImVec2 GameViewWindow::initialMousePos;
ImVec2 GameViewWindow::mouseDragDist;
ImVec2 GameViewWindow::accumulatedMouseDragDist;
ImVec2 GameViewWindow::currentMouseDragDist;

float GameViewWindow::aspectRatioXScale;
float GameViewWindow::aspectRatioYScale;
float GameViewWindow::aspectRatioWidth;
float GameViewWindow::aspectRatioHeight;

//Initialize game viewport system
void GameViewWindow::Initialise() {
	LoadViewportConfigFromJSON(FilePathManager::GetIMGUIViewportJSONPath());
}
bool GameViewWindow::isPaused = false;
//Handle viewport setup, processing and rendering
void GameViewWindow::Update() {
	SetupViewportTexture();

	windowSize = ImGui::GetContentRegionAvail();

	ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	// Add pause button to viewport
	if (ImGui::Button(isPaused ? "Resume" : "Pause")) {
		TogglePause();
	}


	viewportPos = ImGui::GetWindowPos();
	currentMousePos = ImGui::GetMousePos();

	if (GameViewWindow::IsPointInViewport(currentMousePos.x, currentMousePos.y))
		insideViewport = true;
	else
		insideViewport = false;

	ImGuiIO& io = ImGui::GetIO();
	scrollY = io.MouseWheel;
	zoomDelta = scrollY * 0.1f;
	newZoomLevel = GameViewWindow::zoomLevel + zoomDelta;

	if (insideViewport && clickedZoom) {
		zoomLevel = std::min(GameViewWindow::MAX_ZOOM, std::max(GameViewWindow::MIN_ZOOM, newZoomLevel));
	}

	Console::GetLog() << "zoomLevel " << GameViewWindow::zoomLevel << " MAX_ZOOM " << GameViewWindow::MAX_ZOOM
		<< std::endl;
	CaptureMainWindow();

	ImVec2 availWindowSize = GetLargestSizeForViewport();
	ImVec2 renderPos = GetCenteredPosForViewport(availWindowSize);

	ImGui::SetCursorPos(renderPos);
	ImTextureID textureID = (ImTextureID)(intptr_t)viewportTexture;
	ImGui::Image(textureID, availWindowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}
//Clean up resources
void GameViewWindow::Cleanup() {
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

	glBindTexture(GL_TEXTURE_2D, viewportTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//Capture rendered game scene
GLuint GameViewWindow::pausedTexture = 0;

void GameViewWindow::CaptureMainWindow() {
	if (isPaused) {
		if (pausedTexture == 0) {
			// Create new texture for storing the paused frame
			glGenTextures(1, &pausedTexture);
			glBindTexture(GL_TEXTURE_2D, pausedTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Copy from the back buffer to our pause texture
			GLint previousBuffer;
			glGetIntegerv(GL_READ_BUFFER, &previousBuffer);
			glReadBuffer(GL_BACK);

			// Bind pause texture and copy the current frame
			glBindTexture(GL_TEXTURE_2D, pausedTexture);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, viewportWidth, viewportHeight, 0);

			// Restore previous state
			glReadBuffer(previousBuffer);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Copy the paused texture to the viewport texture
		glBindTexture(GL_TEXTURE_2D, viewportTexture);

		// Get the pixels from pausedTexture
		std::vector<unsigned char> pixels(viewportWidth * viewportHeight * 3);
		glBindTexture(GL_TEXTURE_2D, pausedTexture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

		// Put them in viewportTexture
		glBindTexture(GL_TEXTURE_2D, viewportTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		if (pausedTexture != 0) {
			glDeleteTextures(1, &pausedTexture);
			pausedTexture = 0;
		}

		// Normal capture for unpaused state
		glBindTexture(GL_TEXTURE_2D, viewportTexture);

		GLint previousBuffer;
		glGetIntegerv(GL_READ_BUFFER, &previousBuffer);
		glReadBuffer(GL_BACK);

		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, viewportWidth, viewportHeight, 0);

		glReadBuffer(previousBuffer);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//Function scale real time game scene to the size of the game viewport window
ImVec2 GameViewWindow::GetLargestSizeForViewport()
{
	aspectRatioWidth = windowSize.x;
	aspectRatioHeight = aspectRatioWidth / (aspectRatioXScale / aspectRatioYScale);

	float scaledWidth = aspectRatioWidth;
	float scaledHeight = aspectRatioHeight;

	// Use zoomLevel instead of fixed 3.0f multiplier
	scaledWidth = aspectRatioWidth * zoomLevel;
	scaledHeight = aspectRatioHeight * zoomLevel;


	return ImVec2(scaledWidth, scaledHeight);

}

//Function to translate the real time game scene to the center of the game viewport window
ImVec2 GameViewWindow::GetCenteredPosForViewport(ImVec2 aspectSize)
{
	float viewportX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
	float viewportY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

	if (clickedScreenPan && insideViewport && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		ImVec2 Mouse = ImGui::GetMousePos();
		if (!isDragging) {
			isDragging = true;
			initialMousePos = Mouse;
		}
		currentMouseDragDist = { initialMousePos.x - Mouse.x , initialMousePos.y - Mouse.y };
		accumulatedMouseDragDist.x += (currentMouseDragDist.x - mouseDragDist.x);
		accumulatedMouseDragDist.y += (currentMouseDragDist.y - mouseDragDist.y);

		mouseDragDist = currentMouseDragDist;
	}
	else {
		isDragging = false;
		mouseDragDist = { 0.0f,0.0f };
	}

	viewportX -= accumulatedMouseDragDist.x;  // Apply the accumulated offset
	viewportY -= accumulatedMouseDragDist.y;
	return ImVec2(viewportX, viewportY);

}
//Checks if mouse coordinates are in viewport
bool GameViewWindow::IsPointInViewport(double x, double y) {
	return (x - viewportPos.x <= windowSize.x && x - viewportPos.x >= 0
		&& y - viewportPos.y <= windowSize.y && y - viewportPos.y >= 0);
}

// Load the IMGUI viewport configuration values from JSON file
void GameViewWindow::LoadViewportConfigFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

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

	serializer.ReadFloat(viewportPos.x, "GUIViewport.viewportPos.x");
	serializer.ReadFloat(viewportPos.y, "GUIViewport.viewportPos.y");

	serializer.ReadUnsignedInt(viewportTexture, "GUIViewport.viewportTexture");

	serializer.ReadFloat(zoomLevel, "GUIViewport.zoomLevel");

	serializer.ReadFloat(MIN_ZOOM, "GUIViewport.minZoom");
	serializer.ReadFloat(MAX_ZOOM, "GUIViewport.maxZoom");

	serializer.ReadFloat(zoomDelta, "GUIViewport.zoomDelta");
	serializer.ReadFloat(newZoomLevel, "GUIViewport.newZoomLevel");

	serializer.ReadFloat(scrollY, "GUIViewport.scrollY");

	serializer.ReadFloat(currentMousePos.x, "GUIViewport.currentMousePosX");
	serializer.ReadFloat(currentMousePos.y, "GUIViewport.currentMousePosY");

	serializer.ReadFloat(accumulatedMouseDragDist.x, "GUIViewport.accumulatedMouseDragDist.x");
	serializer.ReadFloat(accumulatedMouseDragDist.y, "GUIViewport.accumulatedMouseDragDist.y");

	serializer.ReadFloat(initialMousePos.x, "GUIViewport.initialMousePos.x");
	serializer.ReadFloat(initialMousePos.y, "GUIViewport.initialMousePos.y");

	serializer.ReadFloat(mouseDragDist.x, "GUIViewport.mouseDragDist.x");
	serializer.ReadFloat(mouseDragDist.y, "GUIViewport.mouseDragDist.y");

	serializer.ReadFloat(aspectRatioWidth, "GUIViewport.aspectRatioWidth");
	serializer.ReadFloat(aspectRatioHeight, "GUIViewport.aspectRatioHeight");

	serializer.ReadFloat(aspectRatioXScale, "GUIViewport.aspectRatioXScale");
	serializer.ReadFloat(aspectRatioYScale, "GUIViewport.aspectRatioYScale");
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