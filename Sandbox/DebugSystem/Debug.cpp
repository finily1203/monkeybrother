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
#include "EnemyComponent.h"
#include "MovementComponent.h"
#include "AABBComponent.h"
#include "AnimationComponent.h"
#include "ClosestPlatform.h"

//Variables for DebugSystem
float DebugSystem::fontSize;
ImVec4 DebugSystem::clearColor;
float DebugSystem::textBorderSize;
int DebugSystem::numberOfColumn;
float DebugSystem::ecsTotal;
bool DebugSystem::foundECS = false;
bool DebugSystem::isZooming = false;
bool DebugSystem::isPanning = false;

float DebugSystem::defaultObjScaleX;
float DebugSystem::defaultObjScaleY;

double DebugSystem::coordinateMaxLimitsX;
double DebugSystem::coordinateMaxLimitsY;
double DebugSystem::coordinateMinLimitsX;
double DebugSystem::coordinateMinLimitsY;
double DebugSystem::orientationMaxLimit;
double DebugSystem::orientationMinLimit;
	  
int DebugSystem::numEntitiesToCreate;
char DebugSystem::numBuffer[MAXBUFFERSIZE];
char DebugSystem::sigBuffer[MAXNAMELENGTH];
char DebugSystem::xCoordinatesBuffer[MAXBUFFERSIZE];
char DebugSystem::yCoordinatesBuffer[MAXBUFFERSIZE];
char DebugSystem::xOrientationBuffer[MAXBUFFERSIZE];
char DebugSystem::yOrientationBuffer[MAXBUFFERSIZE];
double DebugSystem::xCoordinates;
double DebugSystem::yCoordinates;
double DebugSystem::xOrientation;
double DebugSystem::yOrientation;

int DebugSystem::objAttributeSliderMaxLength;
int DebugSystem::objAttributeSliderMidLength;
int DebugSystem::objAttributeSliderMinLength;

std::unordered_map<const char*, double> DebugSystem::systemTimes;
double DebugSystem::loopStartTime;
double DebugSystem::loopStartTimeECS;
double DebugSystem::totalLoopTime;
double DebugSystem::totalLoopTimeECS;
double DebugSystem::lastUpdateTime;
std::vector<const char*> DebugSystem::systems;
std::vector<double> DebugSystem::systemGameLoopPercent;
int DebugSystem::systemCount = 0;

float DebugSystem::objWidthSlide;
float DebugSystem::objHeightSlide;
int DebugSystem::objCount;
bool createEntity = false;

float DebugSystem::objSizeXMax;
float DebugSystem::objSizeXMin;
float DebugSystem::objSizeYMax;
float DebugSystem::objSizeYMin;

//Constructor for DebugSystem class
DebugSystem::DebugSystem() : io{ nullptr }, font{ nullptr } {}

//Destructor for DebugSystem class
DebugSystem::~DebugSystem() {}

void DebugSystem::initialise() {
	LoadDebugConfigFromJSON(FilePathManager::GetIMGUIDebugJSONPath());
	IMGUI_CHECKVERSION();
	ImGui::CreateContext(); 
	io = &ImGui::GetIO(); 
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multiple Viewport
	font = io->Fonts->AddFontFromFileTTF(FilePathManager::GetIMGUIFontPath().c_str(), fontSize);  // Load text font file

	ImGui::StyleColorsDark();  // Set theme as dark
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Separator] = clearColor; // seperator's color     
	style.Colors[ImGuiCol_TableBorderStrong] = clearColor; // table's outer border's color
	style.Colors[ImGuiCol_TableBorderLight] = clearColor;  // table's inner border's color
	style.SeparatorTextBorderSize = textBorderSize;
	style.TabBorderSize = textBorderSize;

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
	
		if (ImGui::CollapsingHeader("Performance Data")) {
			static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
				ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

			ImVec2 outerSize = ImVec2(0.0f, ImGui::CalcTextSize("A").x); //To calculate the size of table

			ImGui::Text("FPS: %.1f", GLFWFunctions::fps); //Display FPS

			ImGui::SeparatorText("Performance Viewer");
			ImGui::Text("Number of Systems: %d", systemCount);

			if (ImGui::BeginTable("Performance Data", numberOfColumn, flags, outerSize)) {
				ImGui::TableSetupColumn("Systems");
				ImGui::TableSetupColumn("Game Loop %");
				ImGui::TableHeadersRow();

				// show non-ECS systems and calculate ECS total
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
								ImGui::Text("%9.2f%%", systemGameLoopPercent[i]); // Right-aligned percentage
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
				GameViewWindow::setZoomLevel(1.f);
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

		if (ImGui::CollapsingHeader("Object Creation")) {

			ImGui::AlignTextToFramePadding();

			ImGui::Text("Total no. of objects: %d", objCount);

			objCount = 0;

			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				objCount++;
			}

			const char* items[] = { "Player", "Enemy", "Platform" };
			static int current_item = 0; // Keeps track of selected item

			// Create the combo box
			ImGui::SetNextItemWidth(objAttributeSliderMidLength);
			if (ImGui::BeginCombo("##dropdown", items[current_item])) {
				for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
					const bool is_selected = (current_item == i);
					if (ImGui::Selectable(items[i], is_selected)) {
						current_item = i;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			ImGui::Text("Object type");

			ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
			ImGui::InputText("##Signature", sigBuffer, IM_ARRAYSIZE(sigBuffer));
			ImGui::SameLine();
			ImGui::Text("Name");

			ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
			ImGui::InputText("##count", numBuffer, IM_ARRAYSIZE(numBuffer), ImGuiInputTextFlags_CharsDecimal);
			ImGui::SameLine();
			ImGui::Text("Objects to be created");

			numEntitiesToCreate = std::max(1, atoi(numBuffer));

			ImGui::SetNextItemWidth(objAttributeSliderMinLength);
			ImGui::InputText("##X Pos", xCoordinatesBuffer, IM_ARRAYSIZE(xCoordinatesBuffer));

			ImGui::SameLine();
			ImGui::Text(",");

			ImGui::SameLine();
			ImGui::SetNextItemWidth(objAttributeSliderMinLength);
			ImGui::InputText("##Y Pos", yCoordinatesBuffer, IM_ARRAYSIZE(yCoordinatesBuffer));

			ImGui::SameLine();
			ImGui::Text("Coordinates");

			xCoordinates = std::max(coordinateMinLimitsX, atof(xCoordinatesBuffer));
			yCoordinates = std::max(coordinateMinLimitsY, atof(yCoordinatesBuffer));

			JSONSerializer serializer;
			if (!serializer.Open(FilePathManager::GetEntitiesJSONPath())) {
				std::cout << "Error: could not open file " << FilePathManager::GetEntitiesJSONPath() << std::endl;
				return;
			}
			Entity entityObj;
			std::string entityId;

			nlohmann::json jsonObj = serializer.GetJSONObject();

			if (ImGui::Button("Create Entity")) {
				for (int i = 0; i < numEntitiesToCreate; i++) {
					entityObj = ecsCoordinator.createEntity();

					entityId = sigBuffer;

					// Common components for all entities
					TransformComponent transform{};
					transform.position.SetX(xCoordinates);
					transform.position.SetY(yCoordinates);
					transform.scale.SetX(defaultObjScaleX);
					transform.scale.SetY(defaultObjScaleX);
					/*serializer.ReadObject(transform.mdl_xform, entityId, "entities.transform.localTransform");
					serializer.ReadObject(transform.mdl_to_ndc_xform, entityId, "entities.transform.projectionMatrix");*/
					ecsCoordinator.addComponent(entityObj, transform);

					ObjectCreationCondition(items, current_item, serializer, entityObj, entityId);

					ecsCoordinator.setEntityID(entityObj, entityId);
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Random")) {
				for (int i = 0; i < numEntitiesToCreate; i++) {
					entityObj = ecsCoordinator.createEntity();

					entityId = sigBuffer;

					// Common components for all entities
					TransformComponent transform{};
					transform.position = myMath::Vector2D(ecsCoordinator.getRandomVal(coordinateMinLimitsX, coordinateMaxLimitsX),
										ecsCoordinator.getRandomVal(coordinateMinLimitsY, coordinateMaxLimitsY));
					transform.orientation.SetX(xOrientation);
					transform.orientation.SetY(yOrientation);
					transform.scale.SetX(defaultObjScaleX);
					transform.scale.SetY(defaultObjScaleY);
					ecsCoordinator.addComponent(entityObj, transform);

					ObjectCreationCondition(items, current_item, serializer, entityObj, entityId);

					ecsCoordinator.setEntityID(entityObj, entityId);
				}
			}

			if (ImGui::Button("Remove All Entity")) {
				for (auto entity : ecsCoordinator.getAllLiveEntities()) {
					ecsCoordinator.destroyEntity(entity);
				}
			}


			ImGui::NewLine();
		}
		if (ImGui::CollapsingHeader("Hierachy List")) {
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
				auto signature = ecsCoordinator.getEntityID(entity);

				float posXSlide = transform.position.GetX();
				float posYSlide = transform.position.GetY();

				float orientationSlide = transform.orientation.GetX();

				objWidthSlide = transform.scale.GetX();
				objHeightSlide = transform.scale.GetY();

				ImGui::PushID(entity);

				if (ImGui::TreeNode("Signature: %s", signature.c_str())) {

					ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
					ImGui::SliderFloat("X", &posXSlide, coordinateMinLimitsX, coordinateMaxLimitsX, "%.1f");
					if (ImGui::IsItemActivated) {
						transform.position.SetX(posXSlide);
					}

					ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
					ImGui::SliderFloat("Y", &posYSlide, coordinateMinLimitsY, coordinateMaxLimitsY, "%.1f");
					if (ImGui::IsItemActivated) {
						transform.position.SetY(posYSlide);
					}

					ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
					ImGui::SliderFloat("Orientation", &orientationSlide, orientationMinLimit, orientationMaxLimit, "%.1f");
					if (ImGui::IsItemActivated) {
						transform.orientation.SetX(orientationSlide);
					}

					ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
					ImGui::SliderFloat("Width", &objWidthSlide, objSizeXMin, objSizeXMax, "%.1f");
					if (ImGui::IsItemActivated) {
						transform.scale.SetX(objWidthSlide);
					}
					ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
					ImGui::SliderFloat("Height", &objHeightSlide, objSizeYMin, objSizeYMax, "%.1f");
					if (ImGui::IsItemActivated) {
						transform.scale.SetY(objHeightSlide);
					}

					if (ImGui::Button("Remove")) {
						ecsCoordinator.destroyEntity(entity);
					}
					if (ImGui::Button("Save")) {
						JSONSerializer serializer;
	
						std::string saveFile = GenerateSaveJSONFile(saveCount);
						ecsCoordinator.SaveEntityToJSON(ecsCoordinator, entity, saveFile);

						saveCount++;
						SaveDebugConfigToJSON(FilePathManager::GetIMGUIDebugJSONPath());

						//ecsCoordinator.SaveEntityToJSON(ecsCoordinator, entity, FilePathManager::GetEntitiesJSONPath());
					}

					ImGui::TreePop();
					
				}

				ImGui::PopID();
				ImGui::Separator();
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
	serializer.ReadFloat(fontSize, "Debug.fontSize");
	serializer.ReadFloat(textBorderSize, "Debug.textBorderSize");
	serializer.ReadInt(numberOfColumn, "Debug.numberOfColumn");
	serializer.ReadFloat(ecsTotal, "Debug.ecsTotal");

	serializer.ReadFloat(defaultObjScaleX, "Debug.defaultObjScaleX");
	serializer.ReadFloat(defaultObjScaleY, "Debug.defaultObjScaleY");

	serializer.ReadDouble(coordinateMaxLimitsX, "Debug.coordinateMaxLimitsX");
	serializer.ReadDouble(coordinateMaxLimitsY, "Debug.coordinateMaxLimitsY");
	serializer.ReadDouble(coordinateMinLimitsX, "Debug.coordinateMinLimitsX");
	serializer.ReadDouble(coordinateMinLimitsY, "Debug.coordinateMinLimitsY");
	serializer.ReadDouble(orientationMaxLimit, "Debug.orientationMaxLimit");
	serializer.ReadDouble(orientationMinLimit, "Debug.orientationMinLimit");
	

	serializer.ReadInt(numEntitiesToCreate, "Debug.numEntitiesToCreate");
	serializer.ReadCharArray(numBuffer,MAXBUFFERSIZE, "Debug.numBuffer");
	serializer.ReadCharArray(sigBuffer, MAXNAMELENGTH, "Debug.sigBuffer");
	serializer.ReadCharArray(xCoordinatesBuffer, MAXBUFFERSIZE, "Debug.xCoordinatesBuffer");
	serializer.ReadCharArray(yCoordinatesBuffer, MAXBUFFERSIZE, "Debug.yCoordinatesBuffer");
	serializer.ReadCharArray(xOrientationBuffer, MAXBUFFERSIZE, "Debug.xOrientationBuffer");
	serializer.ReadCharArray(yOrientationBuffer, MAXBUFFERSIZE, "Debug.yOrientationBuffer");
	serializer.ReadDouble(xCoordinates, "Debug.xCoordinates");
	serializer.ReadDouble(yCoordinates, "Debug.yCoordinates");
	serializer.ReadDouble(xOrientation, "Debug.xOrientation");
	serializer.ReadDouble(yOrientation, "Debug.yOrientation");

	serializer.ReadFloat(clearColor.x, "Debug.clearColor.r");
	serializer.ReadFloat(clearColor.y, "Debug.clearColor.g");
	serializer.ReadFloat(clearColor.z, "Debug.clearColor.b");
	serializer.ReadFloat(clearColor.w, "Debug.clearColor.a");

	serializer.ReadFloat(objSizeXMax, "Debug.objSizeXMax");
	serializer.ReadFloat(objSizeXMin, "Debug.objSizeXMin");
	serializer.ReadFloat(objSizeYMax, "Debug.objSizeYMax");
	serializer.ReadFloat(objSizeYMin, "Debug.objSizeYMin");

	serializer.ReadFloat(objWidthSlide, "Debug.objWidthSlide");
	serializer.ReadFloat(objHeightSlide, "Debug.objHeightSlide");
	serializer.ReadInt(objAttributeSliderMaxLength, "Debug.objAttributeSliderMaxLength");
	serializer.ReadInt(objAttributeSliderMidLength, "Debug.objAttributeSliderMidLength");
	serializer.ReadInt(objAttributeSliderMinLength, "Debug.objAttributeSliderMinLength");

	serializer.ReadInt(objCount, "Debug.objCount");

	serializer.ReadDouble(loopStartTime, "Debug.loopStartTime");
	serializer.ReadDouble(loopStartTimeECS, "Debug.loopStartTimeECS");
	serializer.ReadDouble(totalLoopTime, "Debug.totalLoopTime");
	serializer.ReadDouble(totalLoopTimeECS, "Debug.totalLoopTimeECS");
	serializer.ReadDouble(lastUpdateTime, "Debug.lastUpdateTime");

	serializer.ReadInt(systemCount, "Debug.systemCount");

	// ???? object size x max, object size x min ????
	// ???? object size y max, object size y min ????

	serializer.ReadInt(saveCount, "Debug.saveCount");
	serializer.ReadFloat(lastPosX, "Debug.lastPosX");
}

void DebugSystem::SaveDebugConfigToJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();

	serializer.WriteFloat(fontSize, "Debug.fontSize", filename);
	serializer.WriteFloat(textBorderSize, "Debug.textBorderSize", filename);
	serializer.WriteInt(numberOfColumn, "Debug.numberOfColumn", filename);
	serializer.WriteFloat(ecsTotal, "Debug.ecsTotal", filename);

	serializer.WriteFloat(defaultObjScaleX, "Debug.defaultObjScaleX", filename);
	serializer.WriteFloat(defaultObjScaleY, "Debug.defaultObjScaleY", filename);

	serializer.WriteDouble(coordinateMaxLimitsX, "Debug.coordinateMaxLimitsX", filename);
	serializer.WriteDouble(coordinateMaxLimitsY, "Debug.coordinateMaxLimitsY", filename);
	serializer.WriteDouble(coordinateMinLimitsX, "Debug.coordinateMinLimitsX", filename);
	serializer.WriteDouble(coordinateMinLimitsY, "Debug.coordinateMinLimitsY", filename);
	serializer.WriteDouble(orientationMaxLimit, "Debug.orientationMaxLimit", filename);
	serializer.WriteDouble(orientationMinLimit, "Debug.orientationMinLimit", filename);


	serializer.WriteInt(numEntitiesToCreate, "Debug.numEntitiesToCreate", filename);
	serializer.WriteDouble(xCoordinates, "Debug.xCoordinates", filename);
	serializer.WriteDouble(yCoordinates, "Debug.yCoordinates", filename);
	serializer.WriteDouble(xOrientation, "Debug.xOrientation", filename);
	serializer.WriteDouble(yOrientation, "Debug.yOrientation", filename);

	serializer.WriteFloat(clearColor.x, "Debug.clearColor.r", filename);
	serializer.WriteFloat(clearColor.y, "Debug.clearColor.g", filename);
	serializer.WriteFloat(clearColor.z, "Debug.clearColor.b", filename);
	serializer.WriteFloat(clearColor.w, "Debug.clearColor.a", filename);

	serializer.WriteFloat(objWidthSlide, "Debug.widthSlide", filename);
	serializer.WriteFloat(objHeightSlide, "Debug.heightSlide", filename);

	serializer.WriteFloat(objSizeXMax, "Debug.objSizeXMax", filename);
	serializer.WriteFloat(objSizeXMin, "Debug.objSizeXMin", filename);
	serializer.WriteFloat(objSizeYMax, "Debug.objSizeYMax", filename);
	serializer.WriteFloat(objSizeYMin, "Debug.objSizeYMin", filename);

	serializer.WriteFloat(objWidthSlide, "Debug.objWidthSlide", filename);
	serializer.WriteFloat(objHeightSlide, "Debug.objHeightSlide", filename);
	serializer.WriteInt(objAttributeSliderMaxLength, "Debug.objAttributeSliderMaxLength", filename);
	serializer.WriteInt(objAttributeSliderMidLength, "Debug.objAttributeSliderMidLength", filename);
	serializer.WriteInt(objAttributeSliderMinLength, "Debug.objAttributeSliderMinLength", filename);


	serializer.WriteInt(objCount, "Debug.objCount", filename);

	serializer.WriteDouble(loopStartTime, "Debug.loopStartTime", filename);
	serializer.WriteDouble(loopStartTimeECS, "Debug.loopStartTimeECS", filename);
	serializer.WriteDouble(totalLoopTime, "Debug.totalLoopTime", filename);
	serializer.WriteDouble(totalLoopTimeECS, "Debug.totalLoopTimeECS", filename);
	serializer.WriteDouble(lastUpdateTime, "Debug.lastUpdateTime", filename);

	serializer.WriteInt(systemCount, "Debug.systemCount", filename);

	// ???? object size x max, object size x min ????
	// ???? object size y max, object size y min ????
	serializer.WriteInt(saveCount, "Debug.saveCount", filename);
	serializer.WriteFloat(lastPosX, "Debug.lastPosX", filename);
}

std::string DebugSystem::GenerateSaveJSONFile(int& saveNumber)
{
	std::string execPath = FilePathManager::GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\save" + std::to_string(saveNumber) + ".json";

	std::string sourceFilePath;
	if (saveNumber == 1)
	{
		sourceFilePath = FilePathManager::GetEntitiesJSONPath();
	}

	else
	{
		sourceFilePath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\save" + std::to_string(saveCount - 1) + ".json";
	}


	nlohmann::json entitiesJSON;

	std::ifstream sourceFile(sourceFilePath);

	if (sourceFile.is_open())
	{
		sourceFile >> entitiesJSON;
		sourceFile.close();
	}

	std::ofstream outFile(jsonPath);

	if (outFile.is_open())
	{
		outFile << entitiesJSON.dump(2);
		outFile.close();
	}

	else
	{
		std::cout << "Error: could not create file " << jsonPath << std::endl;
		return "";
	}

	return jsonPath;
}

//Check if legacy key is mapped in ImGui key map
static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1; 
}


void DebugSystem::ObjectCreationCondition(const char* items[], int current_item, JSONSerializer& serializer, Entity entityObj, std::string entityId) {
	if (items[current_item] == "Enemy") {

		EnemyComponent enemy{};
		serializer.ReadObject(enemy.isEnemy, entityId, "entities.enemy.isEnemy");
		MovementComponent movement{};
		serializer.ReadObject(movement.speed, entityId, "entities.movement.speed");

		ecsCoordinator.addComponent(entityObj, enemy);
		ecsCoordinator.addComponent(entityObj, movement);

	}
	else if (items[current_item] == "Player") {

		AABBComponent aabb{};
		serializer.ReadObject(aabb.left, entityId, "entities.aabb.left");
		serializer.ReadObject(aabb.right, entityId, "entities.aabb.right");
		serializer.ReadObject(aabb.top, entityId, "entities.aabb.top");
		serializer.ReadObject(aabb.bottom, entityId, "entities.aabb.bottom");

		MovementComponent movement{};
		serializer.ReadObject(movement.speed, entityId, "entities.movement.speed");

		AnimationComponent animation{};
		serializer.ReadObject(animation.isAnimated, entityId, "entities.animation.isAnimated");

		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, movement);
		ecsCoordinator.addComponent(entityObj, animation);

	}
	else if (items[current_item] == "Platform") {

		AABBComponent aabb{};
		serializer.ReadObject(aabb.left, entityId, "entities.aabb.left");
		serializer.ReadObject(aabb.right, entityId, "entities.aabb.right");
		serializer.ReadObject(aabb.top, entityId, "entities.aabb.top");
		serializer.ReadObject(aabb.bottom, entityId, "entities.aabb.bottom");

		ClosestPlatform closestPlatform{};
		serializer.ReadObject(closestPlatform.isClosest, entityId, "entities.closestPlatform.isClosest");

		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, closestPlatform);
		//serializer.ReadObject(transform.orientation, entityId, "entities.transform.orientation");

	}
}


