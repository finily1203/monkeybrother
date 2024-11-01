#include "GUIGameViewport.h"
#include "GlfwFunctions.h"
#include "GUIConsole.h"
float Clamp(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

//Variables for GameViewWindow
int GameViewWindow::viewportHeight;
int GameViewWindow::viewportWidth;
ImVec2 GameViewWindow::viewportPos;
GLuint GameViewWindow::viewportTexture;
ImVec2 GameViewWindow::windowSize;
bool zoomTestFlag = false;
bool GameViewWindow::clickedZoom = false;
bool GameViewWindow::clickedScreenPan = false;

bool insideViewport = false;
ImVec2 GameViewWindow::accumulatedMouseDragDist;
float GameViewWindow::zoomLevel; 
float GameViewWindow::MIN_ZOOM;  // minimum zoom constant
float GameViewWindow::MAX_ZOOM;  //  maximum zoom constant

bool GameViewWindow::isDragging = false;
ImVec2 GameViewWindow::initialMousePos;
ImVec2 GameViewWindow::mouseDragDist;

void GameViewWindow::Initialise() {
	LoadViewportConfigFromJSON(FilePathManager::GetIMGUIViewportJSONPath());
}

void GameViewWindow::Update() {
	SetupViewportTexture();
	//ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	windowSize = ImGui::GetContentRegionAvail();

	ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	viewportPos = ImGui::GetWindowPos();
	ImVec2 textMouse = ImGui::GetMousePos();
	ImVec2 testTest = ImGui::GetContentRegionAvail();
	Console::GetLog() << textMouse.x - viewportPos.x << "," << viewportPos.x << "," << testTest.x
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

	ImVec2 availWindowSize = GetLargestSizeForViewport();
	ImVec2 renderPos = GetCenteredPosForViewport(availWindowSize);

	ImGui::SetCursorPos(renderPos);
	ImTextureID textureID = (ImTextureID)(intptr_t)viewportTexture;
	ImGui::Image(textureID, availWindowSize, ImVec2(0, 1), ImVec2(1, 0));
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
}

static float aspectWidth = 0;
static float aspectHeight = 0;

//Function scale real time game scene to the size of the game viewport window
ImVec2 GameViewWindow::GetLargestSizeForViewport()
{
	aspectWidth = windowSize.x;
	aspectHeight = (aspectWidth / (16.0f / 9.0f));

	static float scaledWidth = aspectWidth;
	static float scaledHeight = aspectHeight;

	
		// Use zoomLevel instead of fixed 3.0f multiplier
	scaledWidth = aspectWidth * zoomLevel;
	scaledHeight = aspectHeight * zoomLevel;
	

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
			setInitialMousePos(Mouse.x, Mouse.y);
		}
		ImVec2 currentDragDistance = { getInitialMousePos().x - Mouse.x ,getInitialMousePos().y - Mouse.y};

		setAccumulatedDragDistance(currentDragDistance.x - getMouseDragDist().x, currentDragDistance.y - getMouseDragDist().y);
		setMouseDragDist(currentDragDistance.x, currentDragDistance.y);
	}
	else {
		isDragging = false;
		setMouseDragDist(0.0f, 0.0f);
	}

	viewportX -= getAccumulatedDragDistance().x;  // Apply the accumulated offset
	viewportY -= getAccumulatedDragDistance().y;
	return ImVec2(viewportX, viewportY);

}

bool GameViewWindow::IsPointInViewport(double x, double y) {
	return (x - viewportPos.x <= windowSize.x && x - viewportPos.x >= 0 
		&& y - viewportPos.y <= windowSize.y && y - viewportPos.y >= 0);
}

// load the IMGUI viewport configuration values from JSON file
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
	// this is for viewport width and height
	serializer.ReadInt(viewportWidth, "GUIViewport.viewportWidth");
	serializer.ReadInt(viewportHeight, "GUIViewport.viewportHeight");

	// this is for viewport position
	serializer.ReadFloat(viewportPos.x, "GUIViewport.viewportPos.x");
	serializer.ReadFloat(viewportPos.y, "GUIViewport.viewportPos.y");

	// this is for viewport texture
	serializer.ReadUnsignedInt(viewportTexture, "GUIViewport.viewportTexture");

	// this is for zoom level
	serializer.ReadFloat(zoomLevel, "GUIViewport.zoomLevel");

	// this is for the minimum and maximum zoom
	serializer.ReadFloat(MIN_ZOOM, "GUIViewport.minZoom");
	serializer.ReadFloat(MAX_ZOOM, "GUIViewport.maxZoom");
}

void GameViewWindow::SaveViewportConfigToJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();

	serializer.WriteInt(viewportWidth, "GUIViewport.viewportWidth");
	serializer.WriteInt(viewportHeight, "GUIViewport.viewportHeight");

	serializer.WriteFloat(lastViewportSize.x, "GUIViewport.lastViewportSize.x");
	serializer.WriteFloat(lastViewportSize.y, "GUIViewport.lastViewportSize.y");

	serializer.WriteFloat(lastAspectSize.x, "GUIViewport.lastAspectSize.x");
	serializer.WriteFloat(lastAspectSize.y, "GUIViewport.lastAspectSize.y");

	serializer.WriteFloat(lastRenderPos.x, "GUIViewport.lastRenderPos.x");
	serializer.WriteFloat(lastRenderPos.y, "GUIViewport.lastRenderPos.y");

	serializer.WriteFloat(viewportPos.x, "GUIViewport.viewportPos.x");
	serializer.WriteFloat(viewportPos.y, "GUIViewport.viewportPos.y");

	serializer.WriteUnsignedInt(viewportTexture, "GUIViewport.viewportTexture");

	serializer.WriteFloat(zoomLevel, "GUIViewport.zoomLevel");

	serializer.WriteFloat(MIN_ZOOM, "GUIViewport.minZoom");
	serializer.WriteFloat(MAX_ZOOM, "GUIViewport.maxZoom");
}
