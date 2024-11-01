/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Debug.cpp
@brief  :  This file contains the function definition of ImGui GUI debugging features which include the debug
		   window, game viewport window, console window, and crash logging system.

*Lew Zong Han Owen (z.lew) : 
		- Integrated ImGui debug window to display FPS, performance viewer, mouse coordinates, and key/mouse input
		  indication
		- Integrated ImGui game viewport window to capture game scene in real time during debug mode
		- Integrated ImGui console window to allow direct custom debugging output in debugging mode
		- Integrated a crash logging system to detect and log custom and standard exceptions into a crash-log 
		  text file
	
File Contributions: Lew Zong Han Owen (100%)

/*_______________________________________________________________________________________________________________*/
#include "Debug.h"
#include "GUIGameViewport.h"
#include "GUIConsole.h"
#include "GlfwFunctions.h"
#include "Crashlog.h"
#include "GlobalCoordinator.h"

static float widthSlide = 5.0f;
static float heightSlide = 5.0f;
static float sizeSlide = 5.0f;

//Variables for DebugSystem
std::unordered_map<const char*, double> DebugSystem::systemTimes;
double DebugSystem::loopStartTime = 0.0;
double DebugSystem::totalLoopTime = 0.0;
double DebugSystem::lastUpdateTime = 0.0;
std::vector<const char*> DebugSystem::systems;
std::vector<double> DebugSystem::systemGameLoopPercent;
int DebugSystem::systemCount = 0;

//Constructor for DebugSystem class
DebugSystem::DebugSystem() : io{ nullptr }, font{ nullptr } {}

//Destructor for DebugSystem class
DebugSystem::~DebugSystem() {}

void DebugSystem::initialise() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext(); 
	io = &ImGui::GetIO(); 
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multiple Viewport
	font = io->Fonts->AddFontFromFileTTF(FilePathManager::GetIMGUIFontPath().c_str(), 20);  // Load text font file

	ImGui::StyleColorsDark();  // Set theme as dark
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Separator] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // seperator's color     
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // table's outer border's color
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);  // table's inner border's color
	style.SeparatorTextBorderSize = 3.0f;
	style.TabBorderSize = 3.0f;

	if (io->BackendPlatformUserData == nullptr) {
		ImGui_ImplGlfw_InitForOpenGL(GLFWFunctions::pWindow, true);
		ImGui_ImplOpenGL3_Init("#version 130");
	}

	GameViewWindow::Initialise();

	lastUpdateTime = glfwGetTime();

}

void DebugSystem::update() {
	if (GLFWFunctions::isGuiOpen) { //F1 key to open imgui GUI
		//std::cout << "GUI Open" << std::endl;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport(); //Set viewport to capture main application window
		//Set next viewport to match the main the viewport 
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		//Remove window rounding, border, and padding
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	
		ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking);
		ImGui::PopStyleVar(3);
		ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		//Creates the dock space
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();

		ImGui::Begin("Debug");
		if (ImGui::CollapsingHeader("Performance Data")) { //Create collapsing header for perfomance data
			static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
				ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

			ImVec2 outerSize = ImVec2(0.0f, ImGui::CalcTextSize("A").x * 5.5f); //To calculate the size of table 

			ImGui::Text("FPS: %.1f", GLFWFunctions::fps); //Display FPS

			ImGui::SeparatorText("Performance Viewer");
			ImGui::Text("Number of Systems: %d", systemCount);

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
			const char* platformWidthLabel = "Test1";
			const char* platformHeightLabel = "Test2";
			const char* playerSizeLabel = "Size";
			ImGui::SeparatorText("Object");

			// Get available width
			float availWidth = ImGui::GetContentRegionAvail().x;

			// Calculate the minimum width for the label
			ImGui::AlignTextToFramePadding();
			float labelWidth = ImGui::CalcTextSize(platformHeightLabel).x + ImGui::GetStyle().ItemInnerSpacing.x;

			//float prevWidth = widthSlide;

			// Set the width for the slider, ensuring it doesn't go below a minimum value
			float sliderWidth = std::max(10.0f, 150.f);
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(platformWidthLabel, &widthSlide, 0.0f, 10.0f, "%.1f");
			//if (prevWidth < widthSlide) {
			//	// This code runs ONLY while the user is actively dragging the slider
			//	// It will stop when they release the mouse button
			//	GLFWFunctions::scale_up_flag = true;
			//}
			//if (ImGui::IsItemDeactivatedAfterEdit()) {
			//	// Runs once when the user releases the slider
			//	GLFWFunctions::scale_up_flag = false;
			//}
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(platformHeightLabel, &heightSlide, 0.0f, 10.0f, "%.1f");

			ImGui::NewLine();
			ImGui::Button("Scale up");
			if (ImGui::IsItemActive()) {
				GLFWFunctions::scale_up_flag = true;
			}
			else {
				GLFWFunctions::scale_up_flag = false;
			}

			ImGui::SameLine();

			ImGui::Button("Scale down");
			if (ImGui::IsItemActive()) {
				GLFWFunctions::scale_down_flag = true;
			}
			else {
				GLFWFunctions::scale_down_flag = false;
			}

			ImGui::Button("Rotate clockwise");
			if (ImGui::IsItemActive()) {
				GLFWFunctions::right_turn_flag = true;
			}
			else {
				GLFWFunctions::right_turn_flag = false;
			}

			ImGui::SameLine();

			ImGui::Button("Rotate anti-clockwise");
			if (ImGui::IsItemActive()) {
				GLFWFunctions::left_turn_flag = true;
			}
			else {
				GLFWFunctions::left_turn_flag = false;
			}

			ImGui::SeparatorText("Player Object");
			ImGui::SetNextItemWidth(sliderWidth);
			ImGui::SliderFloat(playerSizeLabel, &sizeSlide, 0.0f, 10.0f, "%.1f");

			ImGui::NewLine();
			bool createPlayer = ImGui::Button("Create Object");
			if (createPlayer)
			{
				GLFWFunctions::cloneObject = true;
			}
			/*ImGui::SameLine();
			bool destroyPlayer = ImGui::Button("Destroy Object");
			if (destroyPlayer && GLFWFunctions::cloneObject == true)
			{
				GLFWFunctions::cloneObject = false;
			}*/
			
		}
		ImGui::End();

		ImGuiWindowFlags viewportWindowFlags =
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Game Viewport", nullptr, viewportWindowFlags);
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
	
void DebugSystem::cleanup() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

SystemType DebugSystem::getSystem() {
	return DebugSystemType;
}

//Capture start game loop time
void DebugSystem::StartLoop() {
	loopStartTime = glfwGetTime(); //Capture start game loop time
	systemCount = 0;
	systems.clear();
	systemTimes.clear();
}

//Capture total game loop time
void DebugSystem::EndLoop() {

	totalLoopTime = GLFWFunctions::delta_time; 
}

//Capture system's start time loop
void DebugSystem::StartSystemTiming(const char* systemName) {
	systems.push_back(systemName); //Log system name
	systemTimes[systemName] -= glfwGetTime() - loopStartTime; 
	systemCount++; //Log system quantity
}

//Capture system's end time loop
void DebugSystem::EndSystemTiming(const char* systemName) {
	systemTimes[systemName] += glfwGetTime() - loopStartTime; 
}

//Covnert time loop to percentage
double DebugSystem::SystemPercentage(const char* systemName)
{
	auto it = systemTimes.find(systemName);
	if (it != systemTimes.end()) { //Check if system present
		return (it->second / totalLoopTime) * 100.0; 
	}

	return 0.0;
}

// Update system times and percentages
void DebugSystem::UpdateSystemTimes() {
	double currentTime = glfwGetTime();
	if (currentTime - lastUpdateTime >= 1.0) {  // Check if 1 second has passed
		systemGameLoopPercent.clear();
		for (int i{}; i < systemCount; i++) {
			systemGameLoopPercent.push_back(SystemPercentage(systems[i]));
		}
		lastUpdateTime = currentTime;
	}
}

void DebugSystem::LoadDebugConfigFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	// checks if the JSON file can be opened
	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	// retrieve the JSON object from the JSON file
	nlohmann::json currentObj = serializer.GetJSONObject();

	// read all of the data from the JSON object, assign every read
	// data to every elements that needs to be initialized
	serializer.ReadFloat(clear_color.x, "Debug.clearColor.r");
	serializer.ReadFloat(clear_color.y, "Debug.clearColor.g");
	serializer.ReadFloat(clear_color.z, "Debug.clearColor.b");
	serializer.ReadFloat(clear_color.w, "Debug.clearColor.a");

	serializer.ReadFloat(widthSlide, "Debug.widthSlide");
	serializer.ReadFloat(heightSlide, "Debug.heightSlide");
	serializer.ReadFloat(sizeSlide, "Debug.sizeSlide");

	// ???? object count ????

	serializer.ReadDouble(loopStartTime, "Debug.loopStartTime");
	serializer.ReadDouble(totalLoopTime, "Debug.totalLoopTime");
	serializer.ReadDouble(lastUpdateTime, "Debug.lastUpdateTime");

	serializer.ReadInt(systemCount, "Debug.systemCount");

	// ???? object size x max, object size x min ????
	// ???? object size y max, object size y min ????
}

void DebugSystem::SaveDebugConfigFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();

	serializer.WriteFloat(clear_color.x, "Debug.clearColor.r");
	serializer.WriteFloat(clear_color.y, "Debug.clearColor.g");
	serializer.WriteFloat(clear_color.z, "Debug.clearColor.b");
	serializer.WriteFloat(clear_color.w, "Debug.clearColor.a");

	serializer.WriteFloat(widthSlide, "Debug.widthSlide");
	serializer.WriteFloat(heightSlide, "Debug.heightSlide");
	serializer.WriteFloat(sizeSlide, "Debug.sizeSlide");

	// ???? object count ????

	serializer.WriteDouble(loopStartTime, "Debug.loopStartTime");
	serializer.WriteDouble(totalLoopTime, "Debug.totalLoopTime");
	serializer.WriteDouble(lastUpdateTime, "Debug.lastUpdateTime");

	serializer.WriteInt(systemCount, "Debug.systemCount");

	// ???? object size x max, object size x min ????
	// ???? object size y max, object size y min ????
}

//Check if legacy key is mapped in ImGui key map
static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1; 
}



