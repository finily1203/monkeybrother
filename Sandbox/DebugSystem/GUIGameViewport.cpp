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
bool GameViewWindow::clickedZoom = false;
bool GameViewWindow::clickedScreenPan = false;

float GameViewWindow::zoomLevel = 1.f; 
const float GameViewWindow::MIN_ZOOM = 1.f;  // minimum zoom constant
const float GameViewWindow::MAX_ZOOM = 5.f;  //  maximum zoom constant
bool insideViewport = false;
float GameViewWindow::accumulatedDragDistanceX = 0.0f;
float GameViewWindow::accumulatedDragDistanceY = 0.0f;

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
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	viewportPos = ImGui::GetWindowPos();
	ImVec2 textMouse = ImGui::GetMousePos();
	ImVec2 testTest = ImGui::GetContentRegionAvail();
	Console::GetLog() << textMouse.x - viewportPos.x << "," << viewportPos.x << "," << testTest.x
		<< std::endl;
	Console::GetLog() << lastViewportSize.x << ","
		<< std::endl;
	if (GameViewWindow::IsPointInViewport(textMouse.x, textMouse.y))
		insideViewport = true;
	else
		insideViewport = false;
	ImGuiIO& io = ImGui::GetIO();
	float scrollY = io.MouseWheel;
	float zoomDelta = scrollY * 0.1f;
	float newZoomLevel = GameViewWindow::zoomLevel + zoomDelta;
	if(insideViewport && clickedZoom)
	zoomLevel = std::min(GameViewWindow::MAX_ZOOM,
		std::max(GameViewWindow::MIN_ZOOM,
		    newZoomLevel));

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

static float aspectWidth = 0;
static float aspectHeight = 0;

//Function scale real time game scene to the size of the game viewport window
ImVec2 GameViewWindow::GetLargestSizeForViewport()
{
	ImVec2 windowSize = ImGui::GetContentRegionAvail();

	aspectWidth = windowSize.x;
	aspectHeight = (aspectWidth / (16.0f / 9.0f));

	static float scaledWidth = aspectWidth;
	static float scaledHeight = aspectHeight;

	
		// Use zoomLevel instead of fixed 3.0f multiplier
	scaledWidth = aspectWidth * zoomLevel;
	scaledHeight = aspectHeight * zoomLevel;
	

	return ImVec2(scaledWidth, scaledHeight);

}

static bool isDragging = false;
static float initialMouseX = 0.0f;
static float initialMouseY = 0.0f;
static float dragDistanceX = 0;
static float dragDistanceY = 0;

//Function to translate the real time game scene to the center of the game viewport window
ImVec2 GameViewWindow::GetCenteredPosForViewport(ImVec2 aspectSize)
{
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	float viewportX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
	float viewportY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

	if (clickedScreenPan && insideViewport && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		ImVec2 Mouse = ImGui::GetMousePos();
		if (!isDragging) {
			isDragging = true;
			initialMouseX = Mouse.x;
			initialMouseY = Mouse.y;
		}
		float currentDragDistanceX = initialMouseX - Mouse.x;
		accumulatedDragDistanceX += (currentDragDistanceX - dragDistanceX);  // Add the new drag delta
		dragDistanceX = currentDragDistanceX;

		float currentDragDistanceY = initialMouseY - Mouse.y;
		accumulatedDragDistanceY += (currentDragDistanceY - dragDistanceY);  // Add the new drag delta
		dragDistanceY = currentDragDistanceY;
	}
	else {
		isDragging = false;
		dragDistanceX = 0.0f;  // Reset current drag distance when not dragging
		dragDistanceY = 0.0f;
	}

	viewportX -= accumulatedDragDistanceX;  // Apply the accumulated offset
	viewportY -= accumulatedDragDistanceY;
	return ImVec2(viewportX, viewportY);

}

bool GameViewWindow::IsPointInViewport(double x, double y) {
	return (x - viewportPos.x <= lastViewportSize.x && x - viewportPos.x >= 0 
		&& y - viewportPos.y <= lastViewportSize.y && y - viewportPos.y >= 0);
}
