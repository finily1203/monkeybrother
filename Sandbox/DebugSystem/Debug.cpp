/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Debug.cpp
@brief  :  This file contains the function declaration of ImGui main GUI debugging window and it also coordinates the
		   the other ImGui sub systems' such as game viewport, console, and crash logging. It also includes the game's
		   level editor systems such as game viewport camera controls, object creation, hierarchy list, and save
		   and load feature

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui debug window to display FPS, performance viewer, mouse coordinates, and key/mouse input
		  indication
		- Designed the display synergy between all of ImGui's sub systems in the main debugging window
		- Integrated ImGui Object Creation system to allow custom game objects to be created by inputing object-specific
		  properties' data
		- Integrated ImGui Hierarchy List system to display all existing game objects and also allow data modification to
		  them
		- Integrated ImGui game viewport camera controls to zoom and pan current game scene
		- Integrated serialization & deserialization with ImGui to create a saving and loading feature in level
		  editor

*Ian Loi (ian.loi) :
		- Integrated serialization & deserialization functions to initialize variables from json file, which allows
		  saving and loading feature in the level editor

File Contributions: Lew Zong Han Owen (80%)
					Ian Loi           (20%)

/*_______________________________________________________________________________________________________________*/
#define FULL_PERCENTAGE 100
#define _USE_MATH_DEFINES
#include "EngineDefinitions.h"
#include "Debug.h"
#include "GUIGameViewport.h"
#include "GUIConsole.h"
#include "GUIAssetBrowser.h"
#include "GlfwFunctions.h"
#include "Crashlog.h"
#include "GlobalCoordinator.h"
#include "SystemManager.h"
#include "ECSCoordinator.h" 
#include "EnemyComponent.h"
#include "MovementComponent.h"
#include "AABBComponent.h"
#include "AnimationComponent.h"
#include "ClosestPlatform.h"
#include "GUIHierarchyList.h"
#include "GUIObjectCreation.h"
#include "GUIInspector.h"
#include <cmath>

//Variables for DebugSystem
float DebugSystem::fontSize;
ImVec4 DebugSystem::clearColor;
float DebugSystem::textBorderSize;
bool DebugSystem::isZooming;
bool DebugSystem::isPanning;
float DebugSystem::paddingPV;
double DebugSystem::ecsTotal;
bool DebugSystem::foundECS;
int DebugSystem::systemCount;
ImVec2 DebugSystem::mouseWorldPos;
std::string DebugSystem::iniPath;
std::unordered_map<std::string, double> DebugSystem::systemStartTimes;
std::unordered_map<std::string, double> DebugSystem::accumulatedTimes;
double DebugSystem::loopStartTime;
double DebugSystem::totalLoopTime;
double DebugSystem::lastUpdateTime;
std::vector<const char*> DebugSystem::systems;
std::vector<double> DebugSystem::systemGameLoopPercent;
bool DebugSystem::firstFrame;
std::vector<Entity> DebugSystem::newEntities;

//Constructor for DebugSystem class
DebugSystem::DebugSystem() : io{ nullptr }, font{ nullptr } {}

//Destructor for DebugSystem class
DebugSystem::~DebugSystem() {}

//Initialize ImGui settings and configuration
void DebugSystem::initialise() {
	LoadDebugConfigFromJSON(FilePathManager::GetIMGUIDebugJSONPath());
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();

	// Set up ImGui layout file
	iniPath = FilePathManager::GetExecutablePath();
	iniPath = iniPath.substr(0, iniPath.find_last_of("\\/")) + "\\Sandbox\\assets\\imgui\\imgui_layout.ini";
	std::filesystem::create_directories(iniPath.substr(0, iniPath.find_last_of("\\/")));
	io->IniFilename = iniPath.c_str();

	// Configure ImGui
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multiple Viewport
	font = io->Fonts->AddFontFromFileTTF(assetsManager.GetFontPath("Mono").c_str(), fontSize);  // Load text font file

	// Style configuration
	ImGui::StyleColorsDark();  // Set theme as dark
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Separator] = clearColor;             // seperator's color     
	style.Colors[ImGuiCol_TableBorderStrong] = clearColor;     // table's outer border's color
	style.Colors[ImGuiCol_TableBorderLight] = clearColor;      // table's inner border's color
	style.SeparatorTextBorderSize = textBorderSize;
	style.TabBorderSize = textBorderSize;

	// Initialize ImGui backends
	if (io->BackendPlatformUserData == nullptr) {
		ImGui_ImplGlfw_InitForOpenGL(GLFWFunctions::pWindow, true);
		ImGui_ImplOpenGL3_Init("#version 130");
	}

	// Initialize subsystems
	GameViewWindow::Initialise();
	ObjectCreation::Initialise();
	HierarchyList::Initialise();
	Inspector::Initialise();
	AssetBrowser::Initialise();

	lastUpdateTime = glfwGetTime();
}

//Handle rendering of the debug and level editor features
void DebugSystem::update() {
	if (GLFWFunctions::debug_flag) { //1 key to open imgui GUI
		mouseWorldPos = GameViewWindow::GetMouseWorldPosition();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport(); //Set viewport to capture main application window
		//Set next viewport to match the main the viewport 
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		//Remove window rounding, border, and padding
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace", nullptr, windowFlags);
		ImGui::PopStyleVar(3);
		ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);

		ImGui::Begin("Debug");

		//Performance Data formatting
		if (ImGui::CollapsingHeader("Performance Data")) {
			static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
				ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

			ImVec2 outerSize = ImVec2(0.0f, ImGui::CalcTextSize("A").x); //To calculate the size of table

			ImGui::Text("FPS: %.1f", GLFWFunctions::fps); //Display FPS

			ImGui::SeparatorText("Performance Viewer");
			ImGui::Text("Number of Systems: %d", systemCount);

			if (ImGui::BeginTable("Performance Data", 2, flags)) {

				ImGui::TableSetupColumn("Systems");
				ImGui::TableSetupColumn("Game Loop %");
				ImGui::TableHeadersRow();

				foundECS = false;
				ecsTotal = 0.0;

				//Show non-ECS systems
				for (size_t i = 0; i < systems.size(); i++) {
					if (i >= systemGameLoopPercent.size()) break; // Safety check

					const char* systemName = systems[i];

					// Skip ECS systems
					if (strstr(systemName, "ECS") || strcmp(systemName, "EntityComponentSystem") == 0) {
						foundECS = true;
						ecsTotal += systemGameLoopPercent[i];
						continue;
					}

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("%s", systemName);
					ImGui::TableNextColumn();
					ImGui::Text("%.2f%%", systemGameLoopPercent[i]);
				}

				// Show ECS systems
				if (foundECS) {
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					if (ImGui::TreeNode("ECS Systems")) {
						for (size_t i = 0; i < systems.size(); i++) {
							if (i >= systemGameLoopPercent.size()) break; // Safety check

							const char* systemName = systems[i];
							if (strstr(systemName, "ECS") || strcmp(systemName, "EntityComponentSystem") == 0) {
								ImGui::Text("%s:", systemName);
								float normalizedPercentage = static_cast<float>((systemGameLoopPercent[i] / ecsTotal) * FULL_PERCENTAGE);
								ImGui::SameLine(paddingPV);
								ImGui::Text("%6.2f%%", normalizedPercentage);
							}
						}
						ImGui::TreePop();
					}

					ImGui::TableNextColumn();
					ImGui::Text("%.2f%%", ecsTotal);
				}

				ImGui::EndTable();
			}
		}
		//Input Data formatting
		if (ImGui::CollapsingHeader("Input Data")) { //Create collapsing header for input data
			ImGui::SeparatorText("Mouse Coordinates");
			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse position: (%g, %g)", mouseWorldPos.x, mouseWorldPos.y); //Display mouse position data
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

		//Game Viewport Controls formatting
		if (ImGui::CollapsingHeader("Game Viewport Controls")) {
			if (ImGui::Button("Reset Perspective")) {
				myMath::Vector2D initialCamPos{};
				
				for (auto entity : ecsCoordinator.getAllLiveEntities()) {
					if (ecsCoordinator.getEntityID(entity) == "player") {
						auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
						initialCamPos = myMath::Vector2D{ transform.position.GetX(), transform.position.GetY()};
					}
				}
			
				cameraSystem.setCameraPosition(initialCamPos);

				// Reset camera zoom to default value
				float initialZoom = 1.0f; // Adjust this value based on your default zoom
				cameraSystem.setCameraZoom(initialZoom);
			}

			if (ImGui::Button("Zoom")) {
				isZooming = !isZooming;
			}
			if (isZooming) {
				GameViewWindow::setClickedZoom(true);
				ImGui::SameLine();
				ImGui::Text("Zooming");  // Only show text when zooming is active
			}
			else {
				GameViewWindow::setClickedZoom(false);
				ImGui::SameLine();
				ImGui::Text("Not Zooming");
			}

			if (ImGui::Button("Pan")) {
				isPanning = !isPanning;
			}
			if (isPanning) {
				GameViewWindow::setClickedScreenPan(true);
				ImGui::SameLine();
				ImGui::Text("Panning");  // Only show text when zooming is active
			}
			else {
				GameViewWindow::setClickedScreenPan(false);
				ImGui::SameLine();
				ImGui::Text("Not Panning");
			}
		}

		ImGui::End();

		ImGui::Begin("Object Creation");
		ObjectCreation::Update(); //Object Creation system
		ImGui::End();

		ImGui::Begin("Hierarchy List");
		HierarchyList::Update(); //Hierarchy List system
		ImGui::End();

		ImGui::Begin("Game Viewport");
		GameViewWindow::Update(); //Game viewport system
		ImGui::End();

		ImGui::Begin("Inspector");
		Inspector::Update(); //Inspector system
		ImGui::End();

		ImGui::Begin("Console");
		Console::Update("Console"); //ImGui console system
		ImGui::End();

		ImGui::Begin("Assets Browser");
		AssetBrowser::Update();
		ImGui::End();

		ImGui::End();//dockspace

		//Rendering of UI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}
}
//Clean up and shut down ImGui resources	
void DebugSystem::cleanup() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
//For performance viewer
SystemType DebugSystem::getSystem() {
	return DebugSystemType;
}

//Capture start game loop time
void DebugSystem::StartLoop() {
	if (glfwGetTime() - lastUpdateTime >= 1.0f) {
		// Reset accumulated times every second
		accumulatedTimes.clear();
		firstFrame = true;
	}
	loopStartTime = glfwGetTime();
}

//Capture total game loop time
void DebugSystem::EndLoop() {
	totalLoopTime = glfwGetTime() - loopStartTime;
	if (firstFrame) {
		firstFrame = false;
		lastUpdateTime = glfwGetTime();
	}
}

//Capture system's start time loop
void DebugSystem::StartSystemTiming(const char* systemName) {
	if (std::find(systems.begin(), systems.end(), systemName) == systems.end()) {
		systems.push_back(systemName);
		systemCount = static_cast<int>(systems.size());
	}
	systemStartTimes[systemName] = glfwGetTime();
}

//Capture system's end time loop
void DebugSystem::EndSystemTiming(const char* systemName) {
	auto it = systemStartTimes.find(systemName);
	if (it != systemStartTimes.end()) {
		double duration = glfwGetTime() - it->second;
		accumulatedTimes[systemName] += duration;  // Accumulate time
	}
}

// Update system times and percentages
void DebugSystem::UpdateSystemTimes() {
	double currentTime = glfwGetTime();
	if (currentTime - lastUpdateTime >= 1.0f) {

		// Calculate total time spent in all systems
		double totalSystemTime = 0.0;
		for (const auto& pair : accumulatedTimes) {
			totalSystemTime += pair.second;
		}

		// Update percentages
		systemGameLoopPercent.clear();
		systemGameLoopPercent.resize(systems.size());

		for (size_t i = 0; i < systems.size(); i++) {
			const char* systemName = systems[i];
			auto it = accumulatedTimes.find(systemName);
			if (it != accumulatedTimes.end()) {
				double percentage = (it->second / totalSystemTime) * FULL_PERCENTAGE;
				systemGameLoopPercent[i] = percentage;
			}
			else {
				systemGameLoopPercent[i] = 0.0;
			}
		}

		lastUpdateTime = currentTime;
	}
}
//Load debug system configuration from JSON
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

	serializer.ReadBool(isZooming, "Debug.isZooming");
	serializer.ReadBool(isPanning, "Debug.isPanning");
	serializer.ReadBool(firstFrame, "Debug.firstFrame");
	serializer.ReadBool(foundECS, "Debug.foundECS");

	serializer.ReadDouble(ecsTotal, "Debug.ecsTotal");
	serializer.ReadFloat(paddingPV, "Debug.paddingPV");

	serializer.ReadFloat(fontSize, "Debug.fontSize");
	serializer.ReadFloat(textBorderSize, "Debug.textBorderSize");

	serializer.ReadFloat(clearColor.x, "Debug.clearColor.r");
	serializer.ReadFloat(clearColor.y, "Debug.clearColor.g");
	serializer.ReadFloat(clearColor.z, "Debug.clearColor.b");
	serializer.ReadFloat(clearColor.w, "Debug.clearColor.a");

	serializer.ReadDouble(loopStartTime, "Debug.loopStartTime");
	serializer.ReadDouble(totalLoopTime, "Debug.totalLoopTime");
	serializer.ReadDouble(lastUpdateTime, "Debug.lastUpdateTime");

	serializer.ReadInt(systemCount, "Debug.systemCount");
}

//Check if legacy key is mapped in ImGui key map
static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1;
}
