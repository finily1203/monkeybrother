#include "GUIGameViewport.h"
#include "GlfwFunctions.h"
#include "GUIConsole.h"
float Clamp(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

//Variables for GameViewWindow
int GameViewWindow::viewportHeight = 0;
int GameViewWindow::viewportWidth = 0;
ImVec2 GameViewWindow::lastViewportSize = { 0,0 };
ImVec2 GameViewWindow::lastAspectSize = { 0,0 };
ImVec2 GameViewWindow::lastRenderPos = { 0,0 };
//ImVec2 GameViewWindow::windowSize = { 0,0 };
ImVec2 GameViewWindow::viewportPos = { 0, 0 };
GLuint GameViewWindow::viewportTexture = 0;
bool zoomTestFlag = false;

float GameViewWindow::zoomLevel = 1.f; 
const float GameViewWindow::MIN_ZOOM = 1.f;  // minimum zoom constant
const float GameViewWindow::MAX_ZOOM = 5.f;  //  maximum zoom constant

void GameViewWindow::Initialise() {
	viewportWidth = 1600;
	viewportHeight = 900;
	viewportTexture = 0;
}

void GameViewWindow::Update() {
	SetupViewportTexture();

	//ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	viewportPos = ImGui::GetWindowPos();

	Console::GetLog() << "zoomLevel " << GameViewWindow::zoomLevel << " MAX_ZOOM " << GameViewWindow::MAX_ZOOM
		<< std::endl;
	CaptureMainWindow();

	ImVec2 windowSize = GetLargestSizeForViewport();
	ImVec2 renderPos = GetCenteredPosForViewport(windowSize);

	ImGui::SetCursorPos(renderPos);
	ImTextureID textureID = (ImTextureID)(intptr_t)viewportTexture;
	ImGui::Image(textureID, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

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
void GameViewWindow::CaptureMainWindow() {
	glBindTexture(GL_TEXTURE_2D, viewportTexture);

	// Store the current read buffer
	GLint previousBuffer;
	glGetIntegerv(GL_READ_BUFFER, &previousBuffer);

	// Set the read buffer to the back buffer
	glReadBuffer(GL_BACK);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, viewportWidth, viewportHeight, 0);

	// Restore the previous read buffer
	glReadBuffer(previousBuffer);

	glBindTexture(GL_TEXTURE_2D, 0);

	UpdateViewportSize();
}

//Function scale real time game scene to the size of the game viewport window
ImVec2 GameViewWindow::GetLargestSizeForViewport()
{
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	/*windowSize.x -= ImGui::GetScrollX();
	windowSize.y -= ImGui::GetScrollY();*/

	float aspectWidth;
	float aspectHeight;

	aspectWidth = windowSize.x;
	aspectHeight = (aspectWidth / (16.0f / 9.0f));

	if (GLFWFunctions::zoomViewport) {
		// Use zoomLevel instead of fixed 3.0f multiplier
		aspectWidth *= zoomLevel;
		aspectHeight *= zoomLevel;

	}

	/*if (aspectHeight > windowSize.y) {
		aspectHeight = windowSize.y;
		aspectWidth = aspectHeight * (16.0f / 9.0f);
	}*/

	return ImVec2(aspectWidth, aspectHeight);
}

//Function to translate the real time game scene to the center of the game viewport window
ImVec2 GameViewWindow::GetCenteredPosForViewport(ImVec2 aspectSize)
{
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	ImVec2 viewportPos = ImGui::GetCursorScreenPos();
	float viewportX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
	float viewportY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

	if (GLFWFunctions::zoomViewport && !GLFWFunctions::isAtMaxZoom) {
		// Only apply offset if we're not at max zoom
		float localMouseX = GLFWFunctions::zoomMouseCoordX - viewportPos.x;
		float localMouseY = GLFWFunctions::zoomMouseCoordY - viewportPos.y;

		float offsetX = (localMouseX - windowSize.x / 2.0f) * (zoomLevel - 1.0f);
		float offsetY = (localMouseY - windowSize.y / 2.0f) * (zoomLevel - 1.0f);

		viewportX -= offsetX;
		viewportY -= offsetY;
	}

	return ImVec2(viewportX, viewportY);

}

bool GameViewWindow::IsPointInViewport(double x, double y) {

	ImVec2 sceneStart = ImVec2(viewportPos.x + lastRenderPos.x, viewportPos.y + lastRenderPos.y);
	ImVec2 sceneEnd = ImVec2(sceneStart.x + lastAspectSize.x, sceneStart.y + lastAspectSize.y);

	return (x >= sceneStart.x && x <= sceneEnd.x &&
		y >= sceneStart.y && y <= sceneEnd.y);
}
