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
#include "SystemManager.h"
#include "ECSCoordinator.h" 

static float widthSlide = 0.f;
static float heightSlide = 5.0f;
static float sizeSlide = 5.0f;
int objCount = 0;

//Variables for DebugSystem
std::unordered_map<const char*, double> DebugSystem::systemTimes;
double DebugSystem::loopStartTime = 0.0;
double DebugSystem::loopStartTimeECS = 0.0;
double DebugSystem::totalLoopTime = 0.0;
double DebugSystem::totalLoopTimeECS = 0.0;
double DebugSystem::lastUpdateTime = 0.0;
std::vector<const char*> DebugSystem::systems;
std::vector<double> DebugSystem::systemGameLoopPercent;
int DebugSystem::systemCount = 0;

float DebugSystem::objSizeXMax = 5000.0f;
float DebugSystem::objSizeXMin = 100.0f;
float DebugSystem::objSizeYMax = 5000.0f;
float DebugSystem::objSizeYMin = 100.0f;

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
	font = io->Fonts->AddFontFromFileTTF("./DebugSystem/Assets/liberation-mono.ttf", 20);  // Load text font file

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
		//if (ImGui::CollapsingHeader("Performance Data")) { //Create collapsing header for perfomance data
			//static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
				//ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

			//ImVec2 outerSize = ImVec2(0.0f, ImGui::CalcTextSize("A").x * 5.5f); //To calculate the size of table 

		//	ImGui::Text("FPS: %.1f", GLFWFunctions::fps); //Display FPS

		//	ImGui::SeparatorText("Performance Viewer");
		//	ImGui::Text("Number of Systems: %d", systemCount);

		//	if (ImGui::BeginTable("Performance Data", 2, flags, outerSize)) {

		//		ImGui::TableSetupColumn("Systems");
		//		ImGui::TableSetupColumn("Game Loop %");
		//		ImGui::TableHeadersRow();

		//		for (int i{}; i < systemCount && i < systemGameLoopPercent.size(); i++) {

		//			ImGui::TableNextRow();
		//			ImGui::TableNextColumn();
		//			ImGui::Text(systems[i]); //Display system's name
		//			ImGui::TableNextColumn();
		//			ImGui::Text("%.2f%%", systemGameLoopPercent[i]); //Display system's game loop percentage
		//		}

		//		ImGui::EndTable();
		//	}
		//}
			if (ImGui::CollapsingHeader("Performance Data")) {
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

					// Track combined ECS percentage
					float ecsTotal = 0.0f;
					bool foundECS = false;

					// First pass - show non-ECS systems and calculate ECS total
					for (int i = 0; i < systemCount && i < systemGameLoopPercent.size(); i++) {
						const char* systemName = systems[i];

						// Check if this is an ECS system
						if (strstr(systemName, "ECS") || strcmp(systemName, "EntityComponentSystem") == 0) {
							if (strcmp(systemName, "EntityComponentSystem")) {
								ecsTotal = systemGameLoopPercent[i];
							}
							foundECS = true;
							continue; // Skip individual ECS systems
						}

						// Show non-ECS systems normally
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text(systemName);
						ImGui::TableNextColumn();
						ImGui::Text("%.2f%%", systemGameLoopPercent[i]);
					}

					// Show combined ECS entry if we found any ECS systems
					if (foundECS) {
						ImGui::TableNextRow();
						ImGui::TableNextColumn();

						// Create a tree node for ECS Systems
						if (ImGui::TreeNode("ECS Systems")) {
							// Calculate column width for the system names
							float textBaseWidth = ImGui::CalcTextSize("PhysicsColliSystemECS: ").x;

							// Show individual ECS systems as children with aligned percentages
							for (int i = 0; i < systemCount && i < systemGameLoopPercent.size(); i++) {
								const char* systemName = systems[i];
								if (strstr(systemName, "ECS") || strcmp(systemName, "EntityComponentSystem") == 0) {
									ImGui::Text("%s:", systemName);
									ImGui::SameLine(textBaseWidth);
									ImGui::Text("%7.2f%%", systemGameLoopPercent[i]); // Right-aligned percentage
								}
							}
							ImGui::TreePop();
						}

						ImGui::TableNextColumn();
						ImGui::Text("%.2f%%", ecsTotal); // Right-aligned with fixed width
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

		if (ImGui::CollapsingHeader("Game Viewport Controls")) {
			if (ImGui::Button("Reset Perspective")) {
				GameViewWindow::setAccumulatedDragDistance(0, 0);
				GameViewWindow::zoomLevel = 1.f;
			}

			static bool isZooming = false;
			static bool isPanning = false;

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

		if (ImGui::CollapsingHeader("Object Creation")) {

			ImGui::AlignTextToFramePadding();
			static int numEntitiesToCreate = 1;  // Default to 1
			static char numBuffer[8] = "1";
			static char sigBuffer[8] = "";
			static char xCoordinatesBuffer[8] = "0";
			static char yCoordinatesBuffer[8] = "0";
			static double xCoordinates = 0.0;
			static double yCoordinates = 0.0;
			//static char signatureBuffer[8] = "";

			ImGui::Text("Total no. of objects: %d", objCount);

			objCount = 0;

			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				objCount++;
			}

			//ImGui::SetNextItemWidth(100);  // Set width of input field
			//ImGui::InputText("##signature", signatureBuffer, IM_ARRAYSIZE(signatureBuffer));
			//ImGui::SameLine();
			//ImGui::Text("Signature");

			ImGui::SetNextItemWidth(100);  // Set width of input field
			ImGui::InputText("##Signature", sigBuffer, IM_ARRAYSIZE(sigBuffer));
			ImGui::SameLine();
			ImGui::Text("Signature");

			ImGui::SetNextItemWidth(100);  // Set width of input field
			ImGui::InputText("##count", numBuffer, IM_ARRAYSIZE(numBuffer), ImGuiInputTextFlags_CharsDecimal);
			ImGui::SameLine();
			ImGui::Text("Objects to be created");

			numEntitiesToCreate = std::max(1, atoi(numBuffer));

			ImGui::SetNextItemWidth(100);
			ImGui::InputText("##X Pos", xCoordinatesBuffer, IM_ARRAYSIZE(xCoordinatesBuffer));

			ImGui::SameLine();
			ImGui::Text(",");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::InputText("##Y Pos", yCoordinatesBuffer, IM_ARRAYSIZE(yCoordinatesBuffer));

			ImGui::SameLine();
			ImGui::Text("Coordinates");

			xCoordinates = std::max(-800.0, atof(xCoordinatesBuffer));
			yCoordinates = std::max(-450.0, atof(yCoordinatesBuffer));


			if (ImGui::Button("Create")) {
				for (int i = 0; i < numEntitiesToCreate; i++) {
					Entity newEntity = ecsCoordinator.createEntity();
					std::string entityID = sigBuffer;
					TransformComponent transform{};
					transform.position = glm::vec2(
						xCoordinates, yCoordinates
					);
					transform.scale = glm::vec2(100.0f, 100.0f);
					ecsCoordinator.addComponent(newEntity, transform);
					ecsCoordinator.setEntityID(newEntity, entityID);
					


				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Random")) {
				for (int i = 0; i < numEntitiesToCreate; i++) {
					Entity newEntity = ecsCoordinator.createEntity();
					TransformComponent transform{};
					transform.position = glm::vec2(
						glm::vec2(ecsCoordinator.getRandomVal(-800.f, 800.f),
							ecsCoordinator.getRandomVal(-450.f, 450.f))
					);
					transform.scale = glm::vec2(100.0f, 100.0f);
					ecsCoordinator.addComponent(newEntity, transform);
				}
			}

			ImGui::NewLine();
		}
		if (ImGui::CollapsingHeader("Hierachy List")) {
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
				auto signature = ecsCoordinator.getEntityID(entity);

				float posXSlide = transform.position.x;
				float posYSlide = transform.position.y;

				ImGui::PushID(entity);

				widthSlide = transform.scale.x;
				heightSlide = transform.scale.y;

				if (ImGui::TreeNode("Signature: %s", signature.c_str())) {
					ImGui::SetNextItemWidth(200);
					ImGui::InputFloat("X", &posXSlide, 1.f, 10.f);
					if (ImGui::IsItemActive || ImGui::IsItemDeactivatedAfterEdit()) {
						transform.position.x = posXSlide;
					}
					ImGui::SetNextItemWidth(200);
					ImGui::InputFloat("Y", &posYSlide, 1.f, 10.f);
					if (ImGui::IsItemActive || ImGui::IsItemDeactivatedAfterEdit()) {
						transform.position.y = posYSlide;
					}

					ImGui::SliderFloat("Width", &widthSlide, objSizeXMin, objSizeXMax, "%.1f");
					if (ImGui::IsItemActivated) {
						transform.scale.x = widthSlide;
					}
					ImGui::SliderFloat("Height", &heightSlide, objSizeYMin, objSizeYMax, "%.1f");
					if (ImGui::IsItemActivated) {
						transform.scale.y = heightSlide;
					}

					if (ImGui::Button("Remove")) {
						ecsCoordinator.destroyEntity(entity);
					}
					ImGui::TreePop();
				}


				ImVec2 viewportPos = GameViewWindow::getViewportPos();
				ImVec2 TESTEST = ImGui::GetCursorPos();
				ImVec2 Mouse = ImGui::GetMousePos();
				float localMouseX = Mouse.x - viewportPos.x;
				float localMouseY = Mouse.y - viewportPos.y;

				/*if (MouseX == transform.position.x &&
					MouseY == transform.position.y &&
					ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					transform.position.x = MouseX;
					transform.position.y = MouseY;
				}*/

				ImGui::PopID();
				ImGui::Separator();

				Console::GetLog() << TESTEST.x << " Baby" << std::endl;

				Console::GetLog() << signature << " || " << transform.position.x << " || " << transform.position.y
					<< std::endl;
			}

		}

		ImGui::End();
		Console::GetLog() << GLFWFunctions::objMoveMouseCoordX << std::endl;

		ImGuiWindowFlags viewportWindowFlags =
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse;

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

void DebugSystem::StartLoopECS() {
	loopStartTimeECS = glfwGetTime(); //Capture start game loop time
}

//Capture total game loop time
void DebugSystem::EndLoop() {

	totalLoopTime = GLFWFunctions::delta_time; 
}

void DebugSystem::EndLoopECS() {

	totalLoopTimeECS = GLFWFunctions::delta_time;
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

//Check if legacy key is mapped in ImGui key map
static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1; 
}



