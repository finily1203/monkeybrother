#include "Debug.h"

bool DebugSystem::showGUI = false;
bool DebugSystem::checkFrame = false;
std::unordered_map<const char*, double> DebugSystem::systemTimes;
double DebugSystem::loopStartTime = 0.0;
double DebugSystem::totalLoopTime = 0.0;
double DebugSystem::lastUpdateTime = 0.0;
std::vector<const char*> DebugSystem::systems;
std::vector<double> DebugSystem::systemGameLoopPercent;
int DebugSystem::systemCount = 0;

std::ofstream CrashLog::logFile;

int GameViewWindow::viewportHeight = 0;
int GameViewWindow::viewportWidth = 0;
GLuint GameViewWindow::viewportTexture = 0;

DebugSystem::DebugSystem() : io{ nullptr }, font1{ nullptr }, font2{ nullptr } {}

DebugSystem::~DebugSystem() {}

void DebugSystem::Initialise() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      // Enable Multiple Viewport
	font1 = io->Fonts->AddFontFromFileTTF("./Debug/Assets/liberation-mono.ttf", 20);
	font2 = io->Fonts->AddFontFromFileTTF("./Debug/Assets/liberation-mono.ttf", 15);
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Separator] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); //outer border of table
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); //inner border of table
	style.SeparatorTextBorderSize = 3.0f;
	style.TabBorderSize = 3.0f;


	ImGui_ImplGlfw_InitForOpenGL(GLFWFunctions::pWindow, true);

	ImGui_ImplOpenGL3_Init("#version 130");

	GameViewWindow::Initialise();

	lastUpdateTime = glfwGetTime();

}

void DebugSystem::Update() {
	if (!showGUI) {
		return;
	}
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking);
	ImGui::PopStyleVar(3);
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	ImGui::End();
	
	//ImGui::ShowDemoWindow();
	ImGui::Begin("Debug");
		if (ImGui::CollapsingHeader("Performance Data")) {
			static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | /*ImGuiTableFlags_ScrollY |*/
				ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
	
			ImVec2 outer_size = ImVec2(0.0f, ImGui::CalcTextSize("A").x * 5.5f);
			//ImVec2 outer_size = ImVec2(0.0f, 50.f);
	
			//ImGui::PushFont(font1);
			ImGui::Text("FPS: %.1f", GLFWFunctions::fps);
			//ImGui::PopFont();
	
			if (ImGui::BeginTable("DebugTool", 2, flags, outer_size)) {
	
				ImGui::TableSetupColumn("Systems");
				ImGui::TableSetupColumn("Game Loop %");
				ImGui::TableHeadersRow();
				/*for (const auto& [systemName, systemTime] : systemTimes) {
	
				}*/
				//size_t count = std::min(systems.size(), systemGameLoopPercent.size());
				for (int i{}; i < systemCount && i < systemGameLoopPercent.size(); i++) {
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text(systems[i]);
					ImGui::TableNextColumn();
					ImGui::Text("%.2f%%", systemGameLoopPercent[i]);
				}
	
				ImGui::EndTable();
			}
	
		}
	
		if (ImGui::CollapsingHeader("Input Data")) {
			ImGui::SeparatorText("Mouse Coordinates");
			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse position: (%g, %g)", io->MousePos.x, io->MousePos.y);
			else
				ImGui::Text("Mouse position: <INVALID>");
	
			ImGui::SeparatorText("Mouse/Keys Input\n");
			ImGui::Text("Mouse/Key pressed:");
	
			ImGuiKey startKey = (ImGuiKey)0;
			//Check that key exist in ImGuiKey data (includes legacy and modern keys)
			for (ImGuiKey key = startKey; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
			{
				//Don't render if key is typed legacy OR if the key is not pressed (avoid duplication)
				if (LegacyKeyDuplicationCheck(key) //Check if key is the legacy version
					|| !ImGui::IsKeyDown(key)) //Check if key is pressed
					continue; //iterates the next key in ImGuiKey
	
				ImGui::SameLine();
				ImGui::Text("\"%s\"", ImGui::GetKeyName(key));
			}
			ImGui::NewLine();
		}
		if (ImGui::CollapsingHeader("Object Creation")) {
			static int clicked = 0;
			static int i0 = 123;
			float value = 0.0f;
			const char* label = "Width";
			const char* label2 = "Height";
			const char* label3 = "Size";
			ImGui::SeparatorText("Platform Object");
	
			static float f1 = 5.0f;
			static float f2 = 5.0f;
			static float f3 = 5.0f;
			// Get available width
			float availWidth = ImGui::GetContentRegionAvail().x;
	
	
			// Calculate the minimum width for the label
			ImGui::AlignTextToFramePadding();
			float labelWidth = ImGui::CalcTextSize(label2).x + ImGui::GetStyle().ItemInnerSpacing.x;
	
	
			// Set the width for the slider, ensuring it doesn't go below a minimum value
			float sliderWidth = std::max(10.0f, 150.f);
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(label, &f1, 0.0f, 10.0f, "%.1f");
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(label2, &f2, 0.0f, 10.0f, "%.1f");
	
			ImGui::NewLine();
			if (ImGui::Button("Create Platform"))
				clicked++;
			if (clicked & 1)
			{
				ImGui::SameLine();
				ImGui::Text("Platform created");
			}
	
	
			ImGui::SeparatorText("Player Object");
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(label3, &f3, 0.0f, 10.0f, "%.1f");
	
			ImGui::NewLine();
			if (ImGui::Button("Create PLayer"))
				clicked++;
			if (clicked & 1)
			{
				ImGui::SameLine();
				ImGui::Text("Player created");
			}
	
	
		}
	
	ImGui::End();

	ImGui::Begin("Game Viewport");
	GameViewWindow::Update();
	ImGui::End();
	
	//Rendering of UI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	}
	
void DebugSystem::Cleanup() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void DebugSystem::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		ToggleAllWindows(!showGUI);
	}
}

void DebugSystem::ToggleAllWindows(bool show) {
	showGUI = show;
	if (!show) {
		// This will hide all windows
		ImGui::CloseCurrentPopup();
	}
}

void DebugSystem::UpdateSystemTimes() {
	double currentTime = glfwGetTime();
	if (currentTime - lastUpdateTime >= 1.0) {  // Check if 1 second has passed
		systemGameLoopPercent.clear();
		// Update system times and percentages
		for (int i{}; i < systemCount; i++) {
			systemGameLoopPercent.push_back(SystemPercentage(systems[i]));
		}
		lastUpdateTime = currentTime;
	}
}

void DebugSystem::StartLoop() {
	loopStartTime = glfwGetTime();
	systemCount = 0;
	systems.clear();
	systemTimes.clear();
	//systemGameLoopPercent.clear();
}

void DebugSystem::StartSystemTiming(const char* systemName) {
	systems.push_back(systemName);
	systemTimes[systemName] -= glfwGetTime() - loopStartTime;
	systemCount++;
}

void DebugSystem::EndSystemTiming(const char* systemName) {
	systemTimes[systemName] += glfwGetTime() - loopStartTime;
}

void DebugSystem::EndLoop() {
	
	totalLoopTime = GLFWFunctions::delta_time;
}

double DebugSystem::SystemPercentage(const char* systemName){
	auto it = systemTimes.find(systemName);
	if (it != systemTimes.end()) {
		return (it->second / totalLoopTime) * 100.0;
	}
	
	return 0.0;
}

static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1; //Check if legacy key is mapped in ImGui key map
}

void CrashLog::Initialise() {
	logFile.open("crash-log.txt", std::ios::out | std::ios::trunc);
	if (logFile.is_open()) {
		logFile << "Log started at: " << getCurrentTimestamp() << std::endl;
		logFile.flush();
	}
	else {
		std::cerr << "Failed to open crash-log.txt" << std::endl;
	}
}

void CrashLog::Cleanup() {
	if (logFile.is_open()) {
		LogDebugMessage("Log end");
		logFile.close();
	}
}

void CrashLog::LogDebugMessage(const std::string& message, const char* file, int line) {
	if(logFile.is_open()) {
		if (file && line) {
			logFile << "[" << getCurrentTimestamp() << "] " << message << " at " << file << " line " << line << std::endl;
			logFile.flush();
		}
		else logFile << "[" << getCurrentTimestamp() << "] " << message << std::endl;
	}
}

void CrashLog::SignalHandler(int signum) {
	switch (signum) {
	case SIGSEGV:
		CrashLog::LogDebugMessage("Signal: SIGSEV (Segmentation Fault)");
		CrashLog::LogDebugMessage("Program accessed memory it shouldn't (e.g. dereferencing a null pointer)");
		CrashLog::LogDebugMessage("Log end");
		break;
	case SIGABRT:
		CrashLog::LogDebugMessage("Signal: SIGABRT (Abort())");
		CrashLog::LogDebugMessage("Program detected a serious error and called abort()");
		CrashLog::LogDebugMessage("Log end");
		break;
	case SIGFPE:
		CrashLog::LogDebugMessage("Signal: SIGFPE (Floating-point exception)");
		CrashLog::LogDebugMessage("Program detected mathematical error (e.g. division by zero)");
		CrashLog::LogDebugMessage("Log end");
		break;
	case SIGILL:
		CrashLog::LogDebugMessage("SIGILL (Illegal instruction)");
		CrashLog::LogDebugMessage("Program tried to execute an invalid machine instruction");
		CrashLog::LogDebugMessage("Log end");
		break;
	default:
		CrashLog::LogDebugMessage("Unknown signal: " + std::to_string(signum));
	}
	std::exit(signum);
}

void CrashLog::SignalChecks() {
	std::signal(SIGSEGV, SignalHandler);
	std::signal(SIGABRT, SignalHandler);
	std::signal(SIGFPE, SignalHandler);
	std::signal(SIGILL, SignalHandler);
}

void GameViewWindow::Initialise() {
	viewportWidth = 1600;
	viewportHeight = 900;
	viewportTexture = 0;
}

void GameViewWindow::Update() {
	SetupViewportTexture();

	ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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

ImVec2 GameViewWindow::GetLargestSizeForViewport() {
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	windowSize.x -= ImGui::GetScrollX();
	windowSize.y -= ImGui::GetScrollY();

	float aspectWidth = windowSize.x;
	float aspectHeight = (aspectWidth / (16.0f / 9.0f));
	if (aspectHeight > windowSize.y) {
		aspectHeight = windowSize.y;
		aspectWidth = aspectHeight * (16.0f / 9.0f);
	}

	return ImVec2(aspectWidth, aspectHeight);
}

ImVec2 GameViewWindow::GetCenteredPosForViewport(ImVec2 aspectSize) {
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	windowSize.x -= ImGui::GetScrollX();
	windowSize.y -= ImGui::GetScrollY();

	float viewportX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
	float viewportY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

	return ImVec2(viewportX + ImGui::GetCursorPosX(), viewportY + ImGui::GetCursorPosY());
}