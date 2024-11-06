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
#include "EngineDefinitions.h"
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
bool DebugSystem::isZooming = false;
bool DebugSystem::isPanning = false;
bool DebugSystem::isSliding = false;
bool DebugSystem::isRedToggled = false;
bool DebugSystem::isGreenToggled = false;
bool DebugSystem::isBlueToggled = false;
bool isSelectingFile = false;

float DebugSystem::defaultObjScaleX;
float DebugSystem::defaultObjScaleY;

float DebugSystem::coordinateMaxLimitsX;
float DebugSystem::coordinateMaxLimitsY;
float DebugSystem::coordinateMinLimitsX;
float DebugSystem::coordinateMinLimitsY;
float DebugSystem::orientationMaxLimit;
float DebugSystem::orientationMinLimit;

float DebugSystem::fontScaleMaxLimit;
float DebugSystem::fontScaleMinLimit;
float DebugSystem::fontColorMaxLimit;
float DebugSystem::fontColorMinLimit;
float DebugSystem::textBoxMaxLimit;
float DebugSystem::textBoxMinLimit;
	  
int DebugSystem::numEntitiesToCreate;
char DebugSystem::numBuffer[MAXBUFFERSIZE];
char DebugSystem::sigBuffer[MAXNAMELENGTH];
char DebugSystem::textBuffer[MAXTEXTSIZE];
char DebugSystem::textScaleBuffer[MAXBUFFERSIZE];
char DebugSystem::xCoordinatesBuffer[MAXBUFFERSIZE];
char DebugSystem::yCoordinatesBuffer[MAXBUFFERSIZE];
char DebugSystem::xOrientationBuffer[MAXBUFFERSIZE];
char DebugSystem::yOrientationBuffer[MAXBUFFERSIZE];
float DebugSystem::xCoordinates;
float DebugSystem::yCoordinates;
float DebugSystem::xOrientation;
float DebugSystem::yOrientation;
float DebugSystem::textScale;

float DebugSystem::objAttributeSliderMaxLength;
float DebugSystem::objAttributeSliderMidLength;
float DebugSystem::objAttributeSliderMinLength;

std::unordered_map<std::string, double> DebugSystem::systemStartTimes;
std::unordered_map<std::string, double> DebugSystem::systemTotalTimes;
std::unordered_map<std::string, double> DebugSystem::accumulatedTimes;
double DebugSystem::loopStartTime;
double DebugSystem::totalLoopTime;
double DebugSystem::lastUpdateTime;
std::vector<const char*> DebugSystem::systems;
std::vector<double> DebugSystem::systemGameLoopPercent;
bool DebugSystem::firstFrame = true;
int DebugSystem::systemCount = 0;

float DebugSystem::objWidthSlide;
float DebugSystem::objHeightSlide;
int DebugSystem::objCount;
bool createEntity = false;
std::vector<Entity> newEntities;

float DebugSystem::objSizeXMax;
float DebugSystem::objSizeXMin;
float DebugSystem::objSizeYMax;
float DebugSystem::objSizeYMin;

int DebugSystem::saveCount;
float DebugSystem::lastPosX;

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

			if (ImGui::BeginTable("Performance Data", 2, flags)) {
				ImGui::TableSetupColumn("Systems");
				ImGui::TableSetupColumn("Game Loop %");
				ImGui::TableHeadersRow();

				double ecsTotal = 0.0;
				bool foundECS = false;

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
								ImGui::SameLine(260);
								ImGui::Text("%6.2f%%", systemGameLoopPercent[i]);
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

			objCount += static_cast<int>(ecsCoordinator.getAllLiveEntities().size());

			const char* items[] = { "Player", "Enemy", "Platform", "TextBox"};
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

			if (!strcmp(items[current_item], "TextBox")) {
				ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
				ImGui::InputText("##Signature", sigBuffer, IM_ARRAYSIZE(sigBuffer));
				ImGui::SameLine();
				ImGui::Text("Name");

				ImGui::SetNextItemWidth(objAttributeSliderMaxLength);  // Set width of input field
				ImGui::InputText("##Text", textBuffer, IM_ARRAYSIZE(textBuffer));
				ImGui::SameLine();
				ImGui::Text("Text");

				ImGui::SetNextItemWidth(objAttributeSliderMinLength);  // Set width of input field
				ImGui::InputText("##TextScale", textScaleBuffer, IM_ARRAYSIZE(textScaleBuffer));
				ImGui::SameLine();
				ImGui::Text("Scale");

				textScale = std::max(0.1f, strtof(textScaleBuffer, nullptr));

				ImGui::SetNextItemWidth(objAttributeSliderMinLength);
				ImGui::InputText("##X Pos", xCoordinatesBuffer, IM_ARRAYSIZE(xCoordinatesBuffer));

				ImGui::SameLine();
				ImGui::Text(",");

				ImGui::SameLine();
				ImGui::SetNextItemWidth(objAttributeSliderMinLength);
				ImGui::InputText("##Y Pos", yCoordinatesBuffer, IM_ARRAYSIZE(yCoordinatesBuffer));

				ImGui::SameLine();
				ImGui::Text("Coordinates");
			}
			else {
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
			}

			xCoordinates = std::max(coordinateMinLimitsX, strtof(xCoordinatesBuffer,nullptr));
			yCoordinates = std::max(coordinateMinLimitsY, strtof(yCoordinatesBuffer, nullptr));

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

					TransformComponent transform{};
					
					if (!strcmp(items[current_item], "TextBox")) {
						FontComponent fontComp{};
						fontComp.text = textBuffer;
						fontComp.textScale = textScale;
						fontComp.fontId = "Journey";
						fontComp.textBoxWidth = 100.0f;
						ecsCoordinator.addComponent(entityObj, fontComp);

						transform.position.SetX(xCoordinates);
						transform.position.SetY(yCoordinates);
						transform.scale.SetX(defaultObjScaleX);
						transform.scale.SetY(defaultObjScaleX);

					}
					else {
					transform.position.SetX(xCoordinates);
					transform.position.SetY(yCoordinates);
					transform.scale.SetX(defaultObjScaleX);
					transform.scale.SetY(defaultObjScaleX);
					}

					ecsCoordinator.addComponent(entityObj, transform);

					ObjectCreationCondition(items, current_item, serializer, entityObj, entityId);
					newEntities.push_back(entityObj);
					ecsCoordinator.setEntityID(entityObj, entityId);

					//nlohmann::json newEntityJSON = AddNewEntityToJSON(transform, entityId);
					//jsonData["entities"].push_back(newEntityJSON);
				}

				//std::ofstream outFile(FilePathManager::GetEntitiesJSONPath());
				//if (outFile.is_open())
				//{
				//	outFile << jsonData.dump(2);
				//	outFile.close();
				//}
			}
			ImGui::SameLine();

			if (ImGui::Button("Random")) {
				for (int i = 0; i < numEntitiesToCreate; i++) {
					entityObj = ecsCoordinator.createEntity();

					entityId = sigBuffer;

					// Common components for all entities
					TransformComponent transform{};
					if (!strcmp(items[current_item], "TextBox")) {
						FontComponent fontComp{};
						fontComp.text = textBuffer;
						fontComp.textScale = 1.0f;
						fontComp.fontId = "Journey";
						ecsCoordinator.addComponent(entityObj, fontComp);

						transform.position = myMath::Vector2D(ecsCoordinator.getRandomVal(coordinateMinLimitsX, coordinateMaxLimitsX),
							ecsCoordinator.getRandomVal(coordinateMinLimitsY, coordinateMaxLimitsY));
						transform.scale.SetX(defaultObjScaleX);
						transform.scale.SetY(defaultObjScaleX);

					}
					else {
						transform.position = myMath::Vector2D(ecsCoordinator.getRandomVal(coordinateMinLimitsX, coordinateMaxLimitsX),
							ecsCoordinator.getRandomVal(coordinateMinLimitsY, coordinateMaxLimitsY));
						transform.scale.SetX(defaultObjScaleX);
						transform.scale.SetY(defaultObjScaleY);
					}
					
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
				if (ecsCoordinator.hasComponent<FontComponent>(entity)) {
					auto& fontComp = ecsCoordinator.getComponent<FontComponent>(entity);
					auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					auto signature = ecsCoordinator.getEntityID(entity);

					float posXSlide = transform.position.GetX();
					float posYSlide = transform.position.GetY();
					
					float fontScaleSlide = fontComp.textScale;
					float textBoxSlide = fontComp.textBoxWidth;
					float fontColorR = fontComp.color.GetX();
					float fontColorG = fontComp.color.GetY();
					float fontColorB = fontComp.color.GetZ();

					Console::GetLog() << "string: " << fontComp.text << std::endl;

					ImGui::PushID(entity);
					if (ImGui::TreeNode("Signature: %s", signature.c_str())) {

						ImGui::SetNextItemWidth(objAttributeSliderMidLength);
						if (ImGui::SliderFloat("Red", &fontColorR, fontColorMinLimit, fontColorMaxLimit, "%.1f")) {
							fontComp.color.SetX(fontColorR);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMidLength);
						if (ImGui::SliderFloat("Green", &fontColorG, fontColorMinLimit, fontColorMaxLimit, "%.1f")) {
							fontComp.color.SetY(fontColorG);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMidLength);
						if (ImGui::SliderFloat("Blue", &fontColorB, fontColorMinLimit, fontColorMaxLimit, "%.1f")) {
							fontComp.color.SetZ(fontColorB);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("X", &posXSlide, coordinateMinLimitsX, coordinateMaxLimitsX, "%.1f")) {
							transform.position.SetX(posXSlide);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("Y", &posYSlide, coordinateMinLimitsY, coordinateMaxLimitsY, "%.1f")) {
							transform.position.SetY(posYSlide);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("Scale", &fontScaleSlide, fontScaleMinLimit, fontScaleMaxLimit, "%.1f")) {
							fontComp.textScale = fontScaleSlide;
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("Text Box", &textBoxSlide, textBoxMinLimit, textBoxMaxLimit, "%.1f")) {
							fontComp.textBoxWidth = textBoxSlide;
						}
						ImGui::TreePop();

					}
					ImGui::PopID();
					ImGui::Separator();
					
				}else
				{
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
						if (ImGui::SliderFloat("X", &posXSlide, coordinateMinLimitsX, coordinateMaxLimitsX, "%.1f")) {
							transform.position.SetX(posXSlide);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("Y", &posYSlide, coordinateMinLimitsY, coordinateMaxLimitsY, "%.1f")) {
							transform.position.SetY(posYSlide);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("Orientation", &orientationSlide, orientationMinLimit, orientationMaxLimit, "%.1f")) {
							transform.orientation.SetX(orientationSlide);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("Width", &objWidthSlide, objSizeXMin, objSizeXMax, "%.1f")) {
							transform.scale.SetX(objWidthSlide);
						}

						ImGui::SetNextItemWidth(objAttributeSliderMaxLength);
						if (ImGui::SliderFloat("Height", &objHeightSlide, objSizeYMin, objSizeYMax, "%.1f")) {
							transform.scale.SetY(objHeightSlide);
						}

						if (ImGui::Button("Remove")) {
							ecsCoordinator.destroyEntity(entity);
						}

						ImGui::TreePop();

					}

					ImGui::PopID();
					ImGui::Separator();

				}
			}

			if (ImGui::Button("Save")) 
			{
				if (saveCount < 2) {
					std::string saveFile = GenerateSaveJSONFile(saveCount);

					for (auto entity : ecsCoordinator.getAllLiveEntities())
					{
						ecsCoordinator.SaveEntityToJSON(ecsCoordinator, entity, saveFile);
					}

					nlohmann::json jsonData;
					std::ifstream inputFile(saveFile);

					if (inputFile.is_open())
					{
						inputFile >> jsonData;
						inputFile.close();
					}

					for (auto& entity : newEntities) {
						TransformComponent transform = ecsCoordinator.getComponent<TransformComponent>(entity);
						std::string entityId = ecsCoordinator.getEntityID(entity);

						nlohmann::json newEntityJSON = DebugSystem::AddNewEntityToJSON(transform, entityId, ecsCoordinator, entity);

						jsonData["entities"].push_back(newEntityJSON);
					}

					std::ofstream outputFile(saveFile);
					if (outputFile.is_open())
					{
						outputFile << jsonData.dump(2);
						outputFile.close();
					}

					newEntities.clear();
					if (saveCount < 2)
						saveCount++;

					//serializer.WriteInt(saveCount, "Debug.saveCount", FilePathManager::GetIMGUIDebugJSONPath());
					SaveDebugConfigFromJSON(FilePathManager::GetIMGUIDebugJSONPath());
					//ecsCoordinator.SaveEntityToJSON(ecsCoordinator, entity, FilePathManager::GetEntitiesJSONPath());
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Load")) {
				isSelectingFile = true;
			}

			if (isSelectingFile) {
				ImGui::OpenPopup("Load save files");
			}

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			// Create popup modal window
			if (ImGui::BeginPopupModal("Load save files", &isSelectingFile, ImGuiWindowFlags_AlwaysAutoResize)) {
				// Your content goes here
				ImGui::BeginChild("SaveFilesList", ImVec2(300, 200), true);

				if (ImGui::Button("Original File", ImVec2(280, 30))) {
					for (auto entity : ecsCoordinator.getAllLiveEntities()) {
						ecsCoordinator.destroyEntity(entity);
					}
					ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetEntitiesJSONPath());
				}

				for (int i = 1; i < saveCount; i++) {
					char label[32];
					sprintf_s(label, "Save File %d", i);
					int saveNum = saveCount;
					
					if (ImGui::Button(label, ImVec2(280, 30))) {

						if (saveNum == 2) {
							for (auto entity : ecsCoordinator.getAllLiveEntities()) {
								ecsCoordinator.destroyEntity(entity);
							}
							saveNum -= 1;
							ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetSaveJSONPath(saveNum));
						}
					}

					ImGui::Spacing();  // Add space between buttons
				}
				
				ImGui::EndChild();

				// Close button
				if (ImGui::Button("Close")) {
					isSelectingFile = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
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
				double percentage = (it->second / totalSystemTime) * 100.0;
				systemGameLoopPercent[i] = percentage;
			}
			else {
				systemGameLoopPercent[i] = 0.0;
			}
		}

		lastUpdateTime = currentTime;
	}
}

nlohmann::json DebugSystem::AddNewEntityToJSON(TransformComponent& transform, std::string const& entityId, ECSCoordinator& ecs, Entity& entity)
{
	nlohmann::json entityJSON;
	nlohmann::json localTransformJSON = nlohmann::json::array();
	nlohmann::json projectionMatrixJSON = nlohmann::json::array();

	// ID
	entityJSON["id"] = entityId;

	// Transform Component
	entityJSON["transform"]["position"]["x"] = transform.position.GetX();
	entityJSON["transform"]["position"]["y"] = transform.position.GetY();
	entityJSON["transform"]["orientation"]["x"] = transform.orientation.GetX();
	entityJSON["transform"]["orientation"]["y"] = transform.orientation.GetY();
	entityJSON["transform"]["scale"]["x"] = transform.scale.GetX();
	entityJSON["transform"]["scale"]["y"] = transform.scale.GetY();

	// Transform Matrices
	for (int i = 0; i < 3; ++i)
	{
		nlohmann::json transformRowJSON = nlohmann::json::array();
		nlohmann::json projectionRowJSON = nlohmann::json::array();
		for (int j = 0; j < 3; ++j)
		{
			transformRowJSON.push_back(transform.mdl_xform.GetMatrixValue(i, j));
			projectionRowJSON.push_back(transform.mdl_to_ndc_xform.GetMatrixValue(i, j));
		}
		localTransformJSON.push_back(transformRowJSON);
		projectionMatrixJSON.push_back(projectionRowJSON);
	}
	entityJSON["transform"]["localTransform"] = localTransformJSON;
	entityJSON["transform"]["projectionMatrix"] = projectionMatrixJSON;

	// AABB Component
	if (ecs.hasComponent<AABBComponent>(entity))
	{
		AABBComponent& aabb = ecs.getComponent<AABBComponent>(entity);
		entityJSON["aabb"]["left"] = aabb.left;
		entityJSON["aabb"]["right"] = aabb.right;
		entityJSON["aabb"]["top"] = aabb.top;
		entityJSON["aabb"]["bottom"] = aabb.bottom;
	}

	// RigidBody Component
	if (ecs.hasComponent<RigidBodyComponent>(entity))
	{
		RigidBodyComponent& rb = ecs.getComponent<RigidBodyComponent>(entity);
		entityJSON["rigidBody"]["mass"] = rb.mass;
		entityJSON["rigidBody"]["gravityScale"] = rb.gravityScale;
		entityJSON["rigidBody"]["jump"] = rb.jump;
		entityJSON["rigidBody"]["dampening"] = rb.dampening;
		entityJSON["rigidBody"]["velocity"]["x"] = rb.velocity.GetX();
		entityJSON["rigidBody"]["velocity"]["y"] = rb.velocity.GetY();
		entityJSON["rigidBody"]["acceleration"]["x"] = rb.acceleration.GetX();
		entityJSON["rigidBody"]["acceleration"]["y"] = rb.acceleration.GetY();
		entityJSON["rigidBody"]["force"]["x"] = rb.force.GetX();
		entityJSON["rigidBody"]["force"]["y"] = rb.force.GetY();
		entityJSON["rigidBody"]["accumulatedForce"]["x"] = rb.accumulatedForce.GetX();
		entityJSON["rigidBody"]["accumulatedForce"]["y"] = rb.accumulatedForce.GetY();
	}

	// Enemy Component
	if (ecs.hasComponent<EnemyComponent>(entity))
	{
		EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(entity);
		entityJSON["enemy"]["isEnemy"] = enemy.isEnemy;
	}

	// Animation Component
	if (ecs.hasComponent<AnimationComponent>(entity))
	{
		AnimationComponent& anim = ecs.getComponent<AnimationComponent>(entity);
		entityJSON["animation"]["isAnimated"] = anim.isAnimated;
	}

	// ClosestPlatform Component
	if (ecs.hasComponent<ClosestPlatform>(entity))
	{
		ClosestPlatform& platform = ecs.getComponent<ClosestPlatform>(entity);
		entityJSON["closestPlatform"]["isClosest"] = platform.isClosest;
	}

	// Font Component
	if (ecs.hasComponent<FontComponent>(entity))
	{
		FontComponent& font = ecs.getComponent<FontComponent>(entity);
		entityJSON["font"]["text"]["string"] = font.text;
		entityJSON["font"]["text"]["BoxWidth"] = font.textBoxWidth;
		entityJSON["font"]["textScale"]["scale"] = font.textScale;
		entityJSON["font"]["color"]["x"] = font.color.GetX();
		entityJSON["font"]["color"]["y"] = font.color.GetY();
		entityJSON["font"]["color"]["z"] = font.color.GetZ();
		entityJSON["font"]["fontId"]["fontName"] = font.fontId;
	}

	return entityJSON;
}

//nlohmann::json DebugSystem::AddNewEntityToJSON(TransformComponent& transform, std::string const& entityId)
//{
//	nlohmann::json entityJSON;
//	nlohmann::json localTransformJSON = nlohmann::json::array();
//	nlohmann::json projectionMatrixJSON = nlohmann::json::array();
//
//	entityJSON["id"] = entityId;
//
//	entityJSON["transform"]["position"]["x"] = transform.position.GetX();
//	entityJSON["transform"]["position"]["y"] = transform.position.GetY();
//
//	entityJSON["transform"]["orientation"]["x"] = transform.orientation.GetX();
//	entityJSON["transform"]["orientation"]["y"] = transform.orientation.GetY();
//
//	entityJSON["transform"]["scale"]["x"] = transform.scale.GetX();
//	entityJSON["transform"]["scale"]["y"] = transform.scale.GetY();
//
//	for (int i = 0; i < 3; ++i)
//	{
//		nlohmann::json transformRowJSON = nlohmann::json::array();
//		nlohmann::json projectionRowJSON = nlohmann::json::array();
//
//		for (int j = 0; j < 3; ++j)
//		{
//			transformRowJSON.push_back(i == j ? 1.0 : 0.0);
//			projectionRowJSON.push_back(i == j ? 1.0 : 0.0);
//		}
//
//		localTransformJSON.push_back(transformRowJSON);
//		projectionMatrixJSON.push_back(projectionRowJSON);
//	}
//
//	entityJSON["transform"]["localTransform"] = localTransformJSON;
//	entityJSON["transform"]["projectionMatrix"] = projectionMatrixJSON;
//
//	return entityJSON;
//}

void DebugSystem::RemoveEntityFromJSON(std::string const& entityId)
{
	nlohmann::json jsonData;
	std::ifstream inputFile(FilePathManager::GetEntitiesJSONPath());

	if (inputFile.is_open())
	{
		inputFile >> jsonData;
		inputFile.close();
	}

	if (jsonData.contains("entities") && jsonData["entities"].is_array())
	{
		auto& entitiesArrayObj = jsonData["entities"];

		entitiesArrayObj.erase(std::remove_if(entitiesArrayObj.begin(), entitiesArrayObj.end(), [&entityId](nlohmann::json const& entity) {
			return entity.contains("id") && entity["id"] == entityId;
		}), entitiesArrayObj.end());
	}
	
	std::ofstream outFile(FilePathManager::GetEntitiesJSONPath());
	
	if (outFile.is_open())
	{
		outFile << jsonData.dump(2);
		outFile.close();
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

	serializer.ReadFloat(defaultObjScaleX, "Debug.defaultObjScaleX");
	serializer.ReadFloat(defaultObjScaleY, "Debug.defaultObjScaleY");

	serializer.ReadFloat(coordinateMaxLimitsX, "Debug.coordinateMaxLimitsX");
	serializer.ReadFloat(coordinateMaxLimitsY, "Debug.coordinateMaxLimitsY");
	serializer.ReadFloat(coordinateMinLimitsX, "Debug.coordinateMinLimitsX");
	serializer.ReadFloat(coordinateMinLimitsY, "Debug.coordinateMinLimitsY");
	serializer.ReadFloat(orientationMaxLimit, "Debug.orientationMaxLimit");
	serializer.ReadFloat(orientationMinLimit, "Debug.orientationMinLimit");

	serializer.ReadInt(numEntitiesToCreate, "Debug.numEntitiesToCreate");
	serializer.ReadCharArray(numBuffer,MAXBUFFERSIZE, "Debug.numBuffer");
	//serializer.ReadCharArray(sigBuffer, MAXNAMELENGTH, "Debug.sigBuffer");
	serializer.ReadCharArray(textBuffer, MAXTEXTSIZE, "Debug.textBuffer");
	serializer.ReadCharArray(textScaleBuffer, MAXBUFFERSIZE, "Debug.textScaleBuffer");
	serializer.ReadCharArray(xCoordinatesBuffer, MAXBUFFERSIZE, "Debug.xCoordinatesBuffer");
	serializer.ReadCharArray(yCoordinatesBuffer, MAXBUFFERSIZE, "Debug.yCoordinatesBuffer");
	serializer.ReadCharArray(xOrientationBuffer, MAXBUFFERSIZE, "Debug.xOrientationBuffer");
	serializer.ReadCharArray(yOrientationBuffer, MAXBUFFERSIZE, "Debug.yOrientationBuffer");
	//serializer.ReadFloat(xCoordinates, "Debug.xCoordinates");
	//serializer.ReadFloat(yCoordinates, "Debug.yCoordinates");
	serializer.ReadFloat(xOrientation, "Debug.xOrientation");
	serializer.ReadFloat(yOrientation, "Debug.yOrientation");
	serializer.ReadFloat(textScale, "Debug.textScale");

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
	serializer.ReadFloat(objAttributeSliderMaxLength, "Debug.objAttributeSliderMaxLength");
	serializer.ReadFloat(objAttributeSliderMidLength, "Debug.objAttributeSliderMidLength");
	serializer.ReadFloat(objAttributeSliderMinLength, "Debug.objAttributeSliderMinLength");

	serializer.ReadInt(objCount, "Debug.objCount");

	serializer.ReadDouble(loopStartTime, "Debug.loopStartTime");
	serializer.ReadDouble(totalLoopTime, "Debug.totalLoopTime");
	serializer.ReadDouble(lastUpdateTime, "Debug.lastUpdateTime");

	serializer.ReadInt(systemCount, "Debug.systemCount");

	serializer.ReadInt(saveCount, "Debug.saveCount");
	serializer.ReadFloat(lastPosX, "Debug.lastPosX");

	serializer.ReadFloat(fontScaleMaxLimit, "Debug.fontScaleMaxLimit");
	serializer.ReadFloat(fontScaleMinLimit, "Debug.fontScaleMinLimit");
	serializer.ReadFloat(fontColorMaxLimit, "Debug.fontColorMaxLimit");
	serializer.ReadFloat(fontColorMinLimit, "Debug.fontColorMinLimit");
	serializer.ReadFloat(textBoxMaxLimit, "Debug.textBoxMaxLimit");
	serializer.ReadFloat(textBoxMinLimit, "Debug.textBoxMinLimit");
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

	/*serializer.WriteFloat(fontSize, "Debug.fontSize", filename);
	serializer.WriteFloat(textBorderSize, "Debug.textBorderSize", filename);

	serializer.WriteFloat(defaultObjScaleX, "Debug.defaultObjScaleX", filename);
	serializer.WriteFloat(defaultObjScaleY, "Debug.defaultObjScaleY", filename);

	serializer.WriteFloat(coordinateMaxLimitsX, "Debug.coordinateMaxLimitsX", filename);
	serializer.WriteFloat(coordinateMaxLimitsY, "Debug.coordinateMaxLimitsY", filename);
	serializer.WriteFloat(coordinateMinLimitsX, "Debug.coordinateMinLimitsX", filename);
	serializer.WriteFloat(coordinateMinLimitsY, "Debug.coordinateMinLimitsY", filename);
	serializer.WriteFloat(orientationMaxLimit, "Debug.orientationMaxLimit", filename);
	serializer.WriteFloat(orientationMinLimit, "Debug.orientationMinLimit", filename);

	serializer.WriteInt(numEntitiesToCreate, "Debug.numEntitiesToCreate", filename);
	serializer.WriteCharArray(numBuffer, MAXBUFFERSIZE, "Debug.numBuffer", filename);
	serializer.WriteCharArray(sigBuffer, MAXNAMELENGTH, "Debug.sigBuffer", filename);
	serializer.WriteCharArray(xCoordinatesBuffer, MAXBUFFERSIZE, "Debug.xCoordinatesBuffer", filename);
	serializer.WriteCharArray(yCoordinatesBuffer, MAXBUFFERSIZE, "Debug.yCoordinatesBuffer", filename);
	serializer.WriteCharArray(xOrientationBuffer, MAXBUFFERSIZE, "Debug.xOrientationBuffer", filename);
	serializer.WriteCharArray(yOrientationBuffer, MAXBUFFERSIZE, "Debug.yOrientationBuffer", filename);
	serializer.WriteFloat(xCoordinates, "Debug.xCoordinates", filename);
	serializer.WriteFloat(yCoordinates, "Debug.yCoordinates", filename);
	serializer.WriteFloat(xOrientation, "Debug.xOrientation", filename);
	serializer.WriteFloat(yOrientation, "Debug.yOrientation", filename);

	serializer.WriteFloat(clearColor.x, "Debug.clearColor.r", filename);
	serializer.WriteFloat(clearColor.y, "Debug.clearColor.g", filename);
	serializer.WriteFloat(clearColor.z, "Debug.clearColor.b", filename);
	serializer.WriteFloat(clearColor.w, "Debug.clearColor.a", filename);

	serializer.WriteFloat(objSizeXMax, "Debug.objSizeXMax", filename);
	serializer.WriteFloat(objSizeXMin, "Debug.objSizeXMin", filename);
	serializer.WriteFloat(objSizeYMax, "Debug.objSizeYMax", filename);
	serializer.WriteFloat(objSizeYMin, "Debug.objSizeYMin", filename);

	serializer.WriteFloat(objWidthSlide, "Debug.widthSlide", filename);
	serializer.WriteFloat(objHeightSlide, "Debug.heightSlide", filename);
	serializer.WriteFloat(objAttributeSliderMaxLength, "Debug.objAttributeSliderMaxLength", filename);
	serializer.WriteFloat(objAttributeSliderMidLength, "Debug.objAttributeSliderMidLength", filename);
	serializer.WriteFloat(objAttributeSliderMinLength, "Debug.objAttributeSliderMinLength", filename);

	serializer.WriteInt(objCount, "Debug.objCount", filename);

	serializer.WriteDouble(loopStartTime, "Debug.loopStartTime", filename);
	serializer.WriteDouble(totalLoopTime, "Debug.totalLoopTime", filename);
	serializer.WriteDouble(lastUpdateTime, "Debug.lastUpdateTime", filename);

	serializer.WriteInt(systemCount, "Debug.systemCount", filename);*/

	serializer.WriteInt(saveCount, "Debug.saveCount", filename);
	//serializer.WriteFloat(lastPosX, "Debug.lastPosX", filename);
}

std::string DebugSystem::GenerateSaveJSONFile(int& saveNumber)
{
	std::string execPath = FilePathManager::GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\save" + std::to_string(saveNumber) + ".json";
	std::string sourceFilePath;

	sourceFilePath = FilePathManager::GetEntitiesJSONPath();

	/*if (saveNumber == 0)
	{
		sourceFilePath = FilePathManager::GetEntitiesJSONPath();
	}*/

	/*else
	{
		sourceFilePath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\save" + std::to_string(saveNumber - 1) + ".json";
	}*/

	//for (int i = saveNumber - 1; i > 0; --i)
	//{
	//	std::string saveFilePath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\save" + std::to_string(i) + ".json";

	//	if (std::filesystem::exists(saveFilePath))
	//	{
	//		sourceFilePath = saveFilePath;
	//		break;
	//	}
	//}

	//if (sourceFilePath.empty())
	//{
	//	sourceFilePath = FilePathManager::GetEntitiesJSONPath();
	//}

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

int DebugSystem::GetSaveCount()
{
	return saveCount;
}

//Check if legacy key is mapped in ImGui key map
static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1; 
}


void DebugSystem::ObjectCreationCondition(const char* items[], int current_item, JSONSerializer& serializer, Entity entityObj, std::string entityId) {
	if (!strcmp(items[current_item],"Enemy")) {

		EnemyComponent enemy{};
		serializer.ReadObject(enemy.isEnemy, entityId, "entities.enemy.isEnemy");

		MovementComponent movement{};
		serializer.ReadObject(movement.speed, entityId, "entities.movement.speed");

		RigidBodyComponent rigidBody{};
		serializer.ReadObject(rigidBody.mass, entityId, "entities.rigidBody.mass");
		serializer.ReadObject(rigidBody.gravityScale, entityId, "entities.rigidBody.gravityScale");
		serializer.ReadObject(rigidBody.jump, entityId, "entities.rigidBody.jump");
		serializer.ReadObject(rigidBody.dampening, entityId, "entities.rigidBody.dampening");
		serializer.ReadObject(rigidBody.velocity, entityId, "entities.rigidBody.velocity");
		serializer.ReadObject(rigidBody.acceleration, entityId, "entities.rigidBody.acceleration");
		serializer.ReadObject(rigidBody.force, entityId, "entities.rigidBody.force");
		serializer.ReadObject(rigidBody.accumulatedForce, entityId, "entities.rigidBody.accumulatedForce");

		ecsCoordinator.addComponent(entityObj, enemy);
		ecsCoordinator.addComponent(entityObj, movement);
		ecsCoordinator.addComponent(entityObj, rigidBody);

	}
	else if (!strcmp(items[current_item], "Player")) {

		AABBComponent aabb{};
		serializer.ReadObject(aabb.left, entityId, "entities.aabb.left");
		serializer.ReadObject(aabb.right, entityId, "entities.aabb.right");
		serializer.ReadObject(aabb.top, entityId, "entities.aabb.top");
		serializer.ReadObject(aabb.bottom, entityId, "entities.aabb.bottom");

		MovementComponent movement{};
		serializer.ReadObject(movement.speed, entityId, "entities.movement.speed");

		AnimationComponent animation{};
		serializer.ReadObject(animation.isAnimated, entityId, "entities.animation.isAnimated");

		RigidBodyComponent rigidBody{};
		serializer.ReadObject(rigidBody.mass, entityId, "entities.rigidBody.mass");
		serializer.ReadObject(rigidBody.gravityScale, entityId, "entities.rigidBody.gravityScale");
		serializer.ReadObject(rigidBody.jump, entityId, "entities.rigidBody.jump");
		serializer.ReadObject(rigidBody.dampening, entityId, "entities.rigidBody.dampening");
		serializer.ReadObject(rigidBody.velocity, entityId, "entities.rigidBody.velocity");
		serializer.ReadObject(rigidBody.acceleration, entityId, "entities.rigidBody.acceleration");
		serializer.ReadObject(rigidBody.force, entityId, "entities.rigidBody.force");
		serializer.ReadObject(rigidBody.accumulatedForce, entityId, "entities.rigidBody.accumulatedForce");

		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, movement);
		ecsCoordinator.addComponent(entityObj, animation);
		ecsCoordinator.addComponent(entityObj, rigidBody);

		std::cout << ecsCoordinator.getComponent<RigidBodyComponent>(entityObj).dampening<< "," << std::endl;
	}
	else if (!strcmp(items[current_item], "Platform")) {

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


