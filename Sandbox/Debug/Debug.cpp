#include "Debug.h"


std::vector<double> DebugSystem::systemGameLoopPercent;
bool DebugSystem::showGUI = false;
std::unordered_map<const char*, double> DebugSystem::systemTimes;
double DebugSystem::loopStartTime = 0.0;
double DebugSystem::totalLoopTime = 0.0;
double DebugSystem::lastUpdateTime = 0.0;
std::vector<const char*> DebugSystem::systems;
int DebugSystem::systemCount = 0;
std::ofstream CrashLog::logFile;

DebugSystem::DebugSystem() : io{ nullptr }, font1{ nullptr }, font2{ nullptr } {}

DebugSystem::~DebugSystem() {}

void DebugSystem::Initialise() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	
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
	lastUpdateTime = glfwGetTime();
}

void DebugSystem::Update() {
	if (showGUI) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();
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

	
		//Rendering of UI
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(GLFWFunctions::pWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
		showGUI = !showGUI;
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
