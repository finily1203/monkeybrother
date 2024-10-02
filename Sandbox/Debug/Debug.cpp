#include "Debug.h"
#include "GlfwFunctions.h"

//Variables for DebugSystem
std::unordered_map<const char*, double> DebugSystem::systemTimes;
double DebugSystem::loopStartTime = 0.0;
double DebugSystem::totalLoopTime = 0.0;
double DebugSystem::lastUpdateTime = 0.0;
std::vector<const char*> DebugSystem::systems;
std::vector<double> DebugSystem::systemGameLoopPercent;
int DebugSystem::systemCount = 0;

//Variables for CrashLog
std::ofstream CrashLog::logFile;

//Variables for GameViewWindow
int GameViewWindow::viewportHeight = 0;
int GameViewWindow::viewportWidth = 0;
GLuint GameViewWindow::viewportTexture = 0;

//Variables for Console
std::vector<std::string> Console::items;
bool Console::autoScroll = true;
bool Console::autoDelete = true;
float Console::lastScrollY = 0.0f;
Console* Console::instance = nullptr;
std::ostringstream Console::currentLog;

DebugSystem::DebugSystem() : io{ nullptr }, font{ nullptr } {}

DebugSystem::~DebugSystem() {}

void DebugSystem::Initialise() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multiple Viewport
	font = io->Fonts->AddFontFromFileTTF("./Debug/Assets/liberation-mono.ttf", 20); 

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Separator] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); //outer border of table
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);  //inner border of table
	style.SeparatorTextBorderSize = 3.0f;
	style.TabBorderSize = 3.0f;


	ImGui_ImplGlfw_InitForOpenGL(GLFWFunctions::pWindow, true);

	ImGui_ImplOpenGL3_Init("#version 130");

	GameViewWindow::Initialise();

	lastUpdateTime = glfwGetTime();

}

void DebugSystem::Update() {
	if (GLFWFunctions::isGuiOpen) { //F1 key to open imgui GUI
		
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
		ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();

		ImGui::Begin("Debug");
		if (ImGui::CollapsingHeader("Performance Data")) { //Create collapsing header for perfomance data
			static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
				ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

			ImVec2 outerSize = ImVec2(0.0f, ImGui::CalcTextSize("A").x * 5.5f); 

			ImGui::Text("FPS: %.1f", GLFWFunctions::fps); //Display FPS

			ImGui::SeparatorText("Performance Viewer");

			if (ImGui::BeginTable("Performance Data", 2, flags, outerSize)) {
				
				ImGui::TableSetupColumn("Systems");
				ImGui::TableSetupColumn("Game Loop %"); 
				ImGui::TableHeadersRow();

				for (int i{}; i < systemCount && i < systemGameLoopPercent.size(); i++) {
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text(systems[i]); //Display system's name
					ImGui::TableNextColumn();
					ImGui::Text("%.2f%%", systemGameLoopPercent[i]); //Display system's game loop percentage
				}

				ImGui::EndTable();
			}
		}

		if (ImGui::CollapsingHeader("Input Data")) { //Create collapsing header for input data
			ImGui::SeparatorText("Mouse Coordinates");
			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse position: (%g, %g)", io->MousePos.x, io->MousePos.y); //Display mouse position data
			else
				ImGui::Text("Mouse position: <INVALID>");

			ImGui::SeparatorText("Mouse/Keys Input\n");
			ImGui::Text("Mouse/Key pressed:"); //Display mouse/key input data

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
			const char* platformWidthLabel = "Width";
			const char* platformHeightLabel = "Height";
			const char* playerSizeLabel = "Size";
			ImGui::SeparatorText("Platform Object");

			static float widthSlide = 5.0f;
			static float heightSlide = 5.0f;
			static float sizeSlide = 5.0f;
			// Get available width
			float availWidth = ImGui::GetContentRegionAvail().x;

			// Calculate the minimum width for the label
			ImGui::AlignTextToFramePadding();
			float labelWidth = ImGui::CalcTextSize(platformHeightLabel).x + ImGui::GetStyle().ItemInnerSpacing.x;

			// Set the width for the slider, ensuring it doesn't go below a minimum value
			float sliderWidth = std::max(10.0f, 150.f);
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(platformWidthLabel, &widthSlide, 0.0f, 10.0f, "%.1f");
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(platformHeightLabel, &heightSlide, 0.0f, 10.0f, "%.1f");

			ImGui::NewLine();
			bool createPlatform = ImGui::Button("Create Platform");
			if (createPlatform)
			{
				ImGui::SameLine();
				ImGui::Text("Platform created");
			}

			ImGui::SeparatorText("Player Object");
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(playerSizeLabel, &sizeSlide, 0.0f, 10.0f, "%.1f");

			ImGui::NewLine();
			bool createPlayer = ImGui::Button("Create PLayer");
			if (createPlayer)
			{
				ImGui::SameLine();
				ImGui::Text("Player created");
			}
		}
		ImGui::End();

		ImGui::Begin("Game Viewport");
		GameViewWindow::Update(); //Game viewport system
		ImGui::End();

		ImGui::Begin("Console");
		Console::Update("Console"); //ImGui console system
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
}
	
void DebugSystem::Cleanup() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void DebugSystem::StartLoop() {
	loopStartTime = glfwGetTime(); //Capture start game loop time
	systemCount = 0;
	systems.clear();
	systemTimes.clear();
}

void DebugSystem::EndLoop() {

	totalLoopTime = GLFWFunctions::delta_time; //Capture total game loop time
}

void DebugSystem::StartSystemTiming(const char* systemName) {
	systems.push_back(systemName); //Log system name
	systemTimes[systemName] -= glfwGetTime() - loopStartTime; //Capture system's start time loop
	systemCount++; //Log system quantity
}

void DebugSystem::EndSystemTiming(const char* systemName) {
	systemTimes[systemName] += glfwGetTime() - loopStartTime; //Capture system's end time loop
}

double DebugSystem::SystemPercentage(const char* systemName)
{
	auto it = systemTimes.find(systemName);
	if (it != systemTimes.end()) { //Check if system present
		return (it->second / totalLoopTime) * 100.0; //Covnert time loop to percentage
	}

	return 0.0;
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

static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1; //Check if legacy key is mapped in ImGui key map
}

void CrashLog::Initialise() {
	logFile.open("crash-log.txt", std::ios::out | std::ios::trunc); //Create crash log file
	if (logFile.is_open()) {
		//Log start of crash logging
		logFile << "[" << GetCurrentTimestamp() << "] " << "Log started" << std::endl;
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
		if (file && line) { //Log exceptions with file and line location
			logFile << "[" << GetCurrentTimestamp() << "] " << message << " at " << file << " line " << line << std::endl;
			logFile.flush();
		}
		else logFile << "[" << GetCurrentTimestamp() << "] " << message << std::endl;
	}
}

std::string CrashLog::GetCurrentTimestamp() {
	std::time_t now = std::time(nullptr); //Capture PC date and time
	std::tm timeinfo;
	localtime_s(&timeinfo, &now);
	std::ostringstream oss;
	oss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
	return oss.str();
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
	std::signal(SIGSEGV, SignalHandler); //Segmentation signal
	std::signal(SIGABRT, SignalHandler); //Abort signal
	std::signal(SIGFPE, SignalHandler);  //Floating-point signal
	std::signal(SIGILL, SignalHandler);  //Illegal signal
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

ImVec2 GameViewWindow::GetLargestSizeForViewport()
{
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

ImVec2 GameViewWindow::GetCenteredPosForViewport(ImVec2 aspectSize)
{
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	windowSize.x -= ImGui::GetScrollX();
	windowSize.y -= ImGui::GetScrollY();

	float viewportX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
	float viewportY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

	return ImVec2(viewportX + ImGui::GetCursorPosX(), viewportY + ImGui::GetCursorPosY());
}

Console& Console::GetLog() {
	if (instance == nullptr) {
		instance = new Console();
	}
	return *instance;
}

void Console::Update(const char* title) {
	GetLog().DrawImpl(title);
}

void Console::DrawImpl(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title)) {
		ImGui::End();
		return;
	}

	// Options menu
	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-scroll", &autoScroll);
		ImGui::Checkbox("Auto Delete", &autoDelete);
		ImGui::EndPopup();
	}

	// Main window
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();
	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::Separator();

	// Reserve enough left-over height for 1 separator + 1 input text
	const float heightReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -heightReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (clear)
		items.clear();
	if (copy)
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for (const auto& item : items)
		ImGui::TextUnformatted(item.c_str()); //Render log messages
	ImGui::PopStyleVar();

	// Auto-scroll logic
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		autoScroll = true;
	if (ImGui::GetScrollY() < lastScrollY)
		autoScroll = false;
	lastScrollY = ImGui::GetScrollY();

	if (autoScroll && ImGui::GetScrollY() < ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	static char inputBuffer[256] = "";
	if (ImGui::InputText("Input", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (inputBuffer[0]) {
			*this << "> " << inputBuffer << std::endl;
			// Here you would typically parse and execute the command
			*this << "Command executed: " << inputBuffer << std::endl;
		}
		inputBuffer[0] = 0; // Clear the input buffer
	}

	ImGui::SameLine();
	if (ImGui::Button("Submit")) {
		if (inputBuffer[0]) {
			*this << "> " << inputBuffer << std::endl;
			// Here you would typically parse and execute the command
			*this << "Command executed: " << inputBuffer << std::endl;
			inputBuffer[0] = 0; // Clear the input buffer
		}
	}

	ImGui::End();
}



