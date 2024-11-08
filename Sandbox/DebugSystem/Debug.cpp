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
#include <cmath>

//Variables for DebugSystem
float DebugSystem::fontSize;
ImVec4 DebugSystem::clearColor;
float DebugSystem::textBorderSize;
bool DebugSystem::isZooming;
bool DebugSystem::isPanning;
bool DebugSystem::isSliding;
bool DebugSystem::isRedToggled;
bool DebugSystem::isGreenToggled;
bool DebugSystem::isBlueToggled;
bool DebugSystem::isSelectingFile;
int DebugSystem::currentItem;

int DebugSystem::numberOfColumnPV;
float DebugSystem::paddingPV;
double DebugSystem::ecsTotal;
bool DebugSystem::foundECS;
float DebugSystem::initialZoomLevel;
float DebugSystem::initialDragDistX;
float DebugSystem::initialDragDistY;

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
int DebugSystem::minEntitesToCreate;
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
float DebugSystem::textBoxWidth;
float DebugSystem::textScaleMinLimit;

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
bool DebugSystem::firstFrame;
int DebugSystem::systemCount;

float DebugSystem::objWidthSlide;
float DebugSystem::objHeightSlide;
int DebugSystem::objCount;
bool DebugSystem::createEntity;
std::vector<Entity> newEntities;

float DebugSystem::objSizeXMax;
float DebugSystem::objSizeXMin;
float DebugSystem::objSizeYMax;
float DebugSystem::objSizeYMin;

int DebugSystem::saveCount;
int DebugSystem::saveLimit;
float DebugSystem::lastPosX;
float DebugSystem::saveWindowWidth;
float DebugSystem::fileWindowWidth;
float DebugSystem::saveWindowHeight;
float DebugSystem::fileWindowHeight;

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
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multiple Viewport
	font = io->Fonts->AddFontFromFileTTF(assetsManager.GetFontPath("Mono").c_str(), fontSize);  // Load text font file

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

//Handle rendering of the debug and level editor features
void DebugSystem::update() {
	if (GLFWFunctions::debug_flag) { //1 key to open imgui GUI

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

		//Game Viewport Controls formatting
		if (ImGui::CollapsingHeader("Game Viewport Controls")) {
			if (ImGui::Button("Reset Perspective")) {
				GameViewWindow::setAccumulatedDragDistance(initialDragDistX, initialDragDistY);
				GameViewWindow::setZoomLevel(initialZoomLevel);
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

		//Object Creation formatting
		if (ImGui::CollapsingHeader("Object Creation")) {

			ImGui::AlignTextToFramePadding();

			ImGui::Text("Total no. of objects: %d", objCount);

			objCount = 0;

			objCount += static_cast<int>(ecsCoordinator.getAllLiveEntities().size()) - 1;

			const char* items[] = { "Player", "Enemy", "Platform", "TextBox" };

			// Create the combo box
			ImGui::SetNextItemWidth(objAttributeSliderMidLength);
			if (ImGui::BeginCombo("##dropdown", items[currentItem])) {
				for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
					const bool typeSelected = (currentItem == i);
					if (ImGui::Selectable(items[i], typeSelected)) {
						currentItem = i;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			ImGui::Text("Object type");

			if (!strcmp(items[currentItem], "TextBox")) { //Specifically for textbox
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

				textScale = std::max(textScaleMinLimit, strtof(textScaleBuffer, nullptr));

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
			else { //The remaining object types
				ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
				ImGui::InputText("##Signature", sigBuffer, IM_ARRAYSIZE(sigBuffer));
				ImGui::SameLine();
				ImGui::Text("Name");

				ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
				ImGui::InputText("##count", numBuffer, IM_ARRAYSIZE(numBuffer), ImGuiInputTextFlags_CharsDecimal);
				ImGui::SameLine();
				ImGui::Text("Objects to be created");

				numEntitiesToCreate = std::max(minEntitesToCreate, atoi(numBuffer));

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

			xCoordinates = std::max(coordinateMinLimitsX, strtof(xCoordinatesBuffer, nullptr));
			yCoordinates = std::max(coordinateMinLimitsY, strtof(yCoordinatesBuffer, nullptr));

			JSONSerializer serializer;
			if (!serializer.Open(FilePathManager::GetEntitiesJSONPath())) {
				std::cout << "Error: could not open file " << FilePathManager::GetEntitiesJSONPath() << std::endl;
				return;
			}
			Entity entityObj;
			std::string entityId;

			nlohmann::json jsonObj = serializer.GetJSONObject();
			//Entity creation button functionalities
			if (ImGui::Button("Create Entity")) {

				for (int i = 0; i < numEntitiesToCreate; i++) {

					entityObj = ecsCoordinator.createEntity();

					entityId = sigBuffer;

					TransformComponent transform{};

					if (!strcmp(items[currentItem], "TextBox")) {
						FontComponent fontComp{};
						fontComp.text = textBuffer;
						fontComp.textScale = textScale;
						fontComp.fontId = "Journey";
						fontComp.textBoxWidth = textBoxMinLimit;
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

					ObjectCreationCondition(items, currentItem, serializer, entityObj, entityId);
					newEntities.push_back(entityObj);
					ecsCoordinator.setEntityID(entityObj, entityId);


				}


			}
			ImGui::SameLine();

			//For creating entity with random position
			if (ImGui::Button("Random")) {
				for (int i = 0; i < numEntitiesToCreate; i++) {
					entityObj = ecsCoordinator.createEntity();

					entityId = sigBuffer;

					// Common components for all entities
					TransformComponent transform{};
					if (!strcmp(items[currentItem], "TextBox")) {
						FontComponent fontComp{};
						fontComp.text = textBuffer;
						fontComp.textScale = textScale;
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

					ObjectCreationCondition(items, currentItem, serializer, entityObj, entityId);

					ecsCoordinator.setEntityID(entityObj, entityId);
				}
			}

			//Button to destroy all existing entity
			if (ImGui::Button("Remove All Entity")) {
				for (auto entity : ecsCoordinator.getAllLiveEntities()) {
					if (ecsCoordinator.getEntityID(entity) == "placeholderentity") {}
					else
						ecsCoordinator.destroyEntity(entity);
				}
			}


			ImGui::NewLine();
		}
		//Hierarchy List formating
		if (ImGui::CollapsingHeader("Hierarchy List")) {
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				if (ecsCoordinator.getEntityID(entity) == "placeholderentity") {
				}
				else
					if (ecsCoordinator.hasComponent<FontComponent>(entity)) { //TextBox specific data modification feature
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
							if (ImGui::Button("Remove")) {
								ecsCoordinator.destroyEntity(entity);
							}
							ImGui::TreePop();

						}
						ImGui::PopID();
						ImGui::Separator();

					}
					else if (ecsCoordinator.hasComponent<PhysicsComponent>(entity) //Player specific data modification features
						&& !ecsCoordinator.hasComponent<EnemyComponent>(entity)) {
						auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
						auto signature = ecsCoordinator.getEntityID(entity);

						float posXSlide = transform.position.GetX();
						float posYSlide = transform.position.GetY();

						float orientationSlide = transform.orientation.GetX();

						float playerScaleSlide = transform.scale.GetX();

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
							if (ImGui::SliderFloat("Size", &playerScaleSlide, objSizeXMin, objSizeXMax, "%.1f")) {
								transform.scale.SetX(playerScaleSlide);
								transform.scale.SetY(playerScaleSlide);
							}

							if (ImGui::Button("Remove")) {
								ecsCoordinator.destroyEntity(entity);
							}

							ImGui::TreePop();

						}

						ImGui::PopID();
						ImGui::Separator();
					}
					else
					{ //Remaining object's data modification features
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

			if (ImGui::Button("Save")) //Generate 1 new json file and save existing object data in it
			{
				if (saveCount < saveLimit) {
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

						std::cout << "Before AddNewEntityToJSON for " << entityId << ": " << jsonData.dump(2) << std::endl;


						nlohmann::json newEntityJSON = DebugSystem::AddNewEntityToJSON(transform, entityId, ecsCoordinator, entity);

						std::cout << "New entity JSON for " << entityId << ": " << newEntityJSON.dump(2) << std::endl;

						jsonData["entities"].push_back(newEntityJSON);
						std::cout << "After push_back: " << jsonData.dump(2) << std::endl;
					}

					std::ofstream outputFile(saveFile);
					if (outputFile.is_open())
					{
						outputFile << jsonData.dump(2);
						outputFile.close();
					}

					newEntities.clear();
					if (saveCount < saveLimit)
						saveCount++;

					SaveDebugConfigFromJSON(FilePathManager::GetIMGUIDebugJSONPath());
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

			// Create popup modal window for loading of save files
			if (ImGui::BeginPopupModal("Load save files", &isSelectingFile, ImGuiWindowFlags_AlwaysAutoResize)) {

				ImGui::BeginChild("SaveFilesList", ImVec2(saveWindowWidth, saveWindowHeight), true);

				if (ImGui::Button("Original File", ImVec2(fileWindowWidth, fileWindowHeight))) {

					for (auto entity : ecsCoordinator.getAllLiveEntities()) {
						ecsCoordinator.destroyEntity(entity);
					}
					ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetEntitiesJSONPath());
				}

				for (int i = 1; i < saveCount; i++) {
					char label[32];
					sprintf_s(label, "Save File %d", i);
					int saveNum = saveCount;

					if (ImGui::Button(label, ImVec2(fileWindowWidth, fileWindowHeight))) {

						if (saveNum == saveLimit) {
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

		ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse);
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
//Add new entity's data to JSON save file
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
	if (ecs.hasComponent<PhysicsComponent>(entity))
	{
		PhysicsComponent& rb = ecs.getComponent<PhysicsComponent>(entity);
		entityJSON["PhysicsComponent"]["velocity"]["x"] = rb.velocity.GetX();
		entityJSON["PhysicsComponent"]["velocity"]["y"] = rb.velocity.GetY();
		entityJSON["PhysicsComponent"]["gravityScale"]["x"] = rb.gravityScale.GetX();
		entityJSON["PhysicsComponent"]["gravityScale"]["y"] = rb.gravityScale.GetY();
		entityJSON["PhysicsComponent"]["acceleration"]["x"] = rb.acceleration.GetX();
		entityJSON["PhysicsComponent"]["acceleration"]["y"] = rb.acceleration.GetY();
		entityJSON["PhysicsComponent"]["accumulatedForce"]["x"] = rb.accumulatedForce.GetX();
		entityJSON["PhysicsComponent"]["accumulatedForce"]["y"] = rb.accumulatedForce.GetY();

		entityJSON["PhysicsComponent"]["jump"] = rb.jump;
		entityJSON["PhysicsComponent"]["dampening"] = rb.dampening;
		entityJSON["PhysicsComponent"]["mass"] = rb.mass;
		entityJSON["PhysicsComponent"]["maxVelocity"] = rb.maxVelocity;
		entityJSON["PhysicsComponent"]["maxAccumulatedForce"] = rb.maxAccumulatedForce;
		entityJSON["PhysicsComponent"]["prevForce"] = rb.prevForce;
		entityJSON["PhysicsComponent"]["targetForce"] = rb.targetForce;

		entityJSON["PhysicsComponent"]["force"]["direction"]["x"] = rb.force.GetDirection().GetX();
		entityJSON["PhysicsComponent"]["force"]["direction"]["y"] = rb.force.GetDirection().GetY();
		entityJSON["PhysicsComponent"]["force"]["magnitude"] = rb.force.GetMagnitude();

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
		FontComponent& fontComp = ecs.getComponent<FontComponent>(entity);
		entityJSON["font"]["text"]["string"] = fontComp.text;
		entityJSON["font"]["text"]["BoxWidth"] = fontComp.textBoxWidth;
		entityJSON["font"]["textScale"]["scale"] = fontComp.textScale;
		entityJSON["font"]["color"]["x"] = fontComp.color.GetX();
		entityJSON["font"]["color"]["y"] = fontComp.color.GetY();
		entityJSON["font"]["color"]["z"] = fontComp.color.GetZ();
		entityJSON["font"]["fontId"]["fontName"] = fontComp.fontId;
	}

	return entityJSON;
}
//Remove an entity from JSON save file
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
	serializer.ReadBool(isSliding, "Debug.isSliding");
	serializer.ReadBool(isRedToggled, "Debug.isRedToggled");
	serializer.ReadBool(isGreenToggled, "Debug.isGreenToggled");
	serializer.ReadBool(isBlueToggled, "Debug.isBlueToggled");
	serializer.ReadBool(isSelectingFile, "Debug.isSelectingFile");
	serializer.ReadBool(createEntity, "Debug.createEntity");
	serializer.ReadBool(firstFrame, "Debug.firstFrame");
	serializer.ReadBool(foundECS, "Debug.foundECS");

	serializer.ReadDouble(ecsTotal, "Debug.ecsTotal");
	serializer.ReadInt(numberOfColumnPV, "Debug.numberOfColumnPV");
	serializer.ReadFloat(paddingPV, "Debug.paddingPV");
	serializer.ReadFloat(initialZoomLevel, "Debug.initialZoomLevel");
	serializer.ReadFloat(initialDragDistX, "Debug.initialDragDistX");
	serializer.ReadFloat(initialDragDistY, "Debug.initialDragDistY");

	serializer.ReadFloat(fontSize, "Debug.fontSize");
	serializer.ReadFloat(textBorderSize, "Debug.textBorderSize");
	serializer.ReadFloat(textScaleMinLimit, "Debug.textScaleMinLimit");

	serializer.ReadFloat(defaultObjScaleX, "Debug.defaultObjScaleX");
	serializer.ReadFloat(defaultObjScaleY, "Debug.defaultObjScaleY");

	serializer.ReadFloat(coordinateMaxLimitsX, "Debug.coordinateMaxLimitsX");
	serializer.ReadFloat(coordinateMaxLimitsY, "Debug.coordinateMaxLimitsY");
	serializer.ReadFloat(coordinateMinLimitsX, "Debug.coordinateMinLimitsX");
	serializer.ReadFloat(coordinateMinLimitsY, "Debug.coordinateMinLimitsY");
	serializer.ReadFloat(orientationMaxLimit, "Debug.orientationMaxLimit");
	serializer.ReadFloat(orientationMinLimit, "Debug.orientationMinLimit");

	serializer.ReadInt(currentItem, "Debug.currentItem");
	serializer.ReadInt(minEntitesToCreate, "Debug.minEntitesToCreate");
	serializer.ReadInt(numEntitiesToCreate, "Debug.numEntitiesToCreate");
	serializer.ReadCharArray(numBuffer, MAXBUFFERSIZE, "Debug.numBuffer");
	serializer.ReadCharArray(textBuffer, MAXTEXTSIZE, "Debug.textBuffer");
	serializer.ReadCharArray(textScaleBuffer, MAXBUFFERSIZE, "Debug.textScaleBuffer");
	serializer.ReadCharArray(xCoordinatesBuffer, MAXBUFFERSIZE, "Debug.xCoordinatesBuffer");
	serializer.ReadCharArray(yCoordinatesBuffer, MAXBUFFERSIZE, "Debug.yCoordinatesBuffer");
	serializer.ReadCharArray(xOrientationBuffer, MAXBUFFERSIZE, "Debug.xOrientationBuffer");
	serializer.ReadCharArray(yOrientationBuffer, MAXBUFFERSIZE, "Debug.yOrientationBuffer");
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

	serializer.ReadInt(saveLimit, "Debug.saveLimit");
	serializer.ReadInt(saveCount, "Debug.saveCount");
	serializer.ReadFloat(lastPosX, "Debug.lastPosX");
	serializer.ReadFloat(fileWindowWidth, "Debug.fileWindowWidth");
	serializer.ReadFloat(saveWindowHeight, "Debug.saveWindowHeight");
	serializer.ReadFloat(fileWindowHeight, "Debug.fileWindowHeight");
	serializer.ReadFloat(saveWindowWidth, "Debug.saveWindowWidth");

	serializer.ReadFloat(fontScaleMaxLimit, "Debug.fontScaleMaxLimit");
	serializer.ReadFloat(fontScaleMinLimit, "Debug.fontScaleMinLimit");
	serializer.ReadFloat(fontColorMaxLimit, "Debug.fontColorMaxLimit");
	serializer.ReadFloat(fontColorMinLimit, "Debug.fontColorMinLimit");
	serializer.ReadFloat(textBoxMaxLimit, "Debug.textBoxMaxLimit");
	serializer.ReadFloat(textBoxMinLimit, "Debug.textBoxMinLimit");
}
//Save debug saveCount data back into same JSON file
void DebugSystem::SaveDebugConfigFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	nlohmann::json jsonObj = serializer.GetJSONObject();

	serializer.WriteInt(saveCount, "Debug.saveCount", filename);

}
//Generates a new JSON save file
std::string DebugSystem::GenerateSaveJSONFile(int& saveNumber)
{
	std::string execPath = FilePathManager::GetExecutablePath();
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\save" + std::to_string(saveNumber) + ".json";
	std::string sourceFilePath;

	sourceFilePath = FilePathManager::GetEntitiesJSONPath();

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
//Get current save count
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

//Creates appropriate components based on object type
void DebugSystem::ObjectCreationCondition(const char* items[], int itemIndex, JSONSerializer& serializer, Entity entityObj, std::string entityId) {
	if (!strcmp(items[itemIndex], "Enemy")) {

		EnemyComponent enemy{};
		serializer.ReadObject(enemy.isEnemy, entityId, "entities.enemy.isEnemy");

		MovementComponent movement{};
		serializer.ReadObject(movement.speed, entityId, "entities.movement.speed");

		PhysicsComponent forces{};

		myMath::Vector2D direction = forces.force.GetDirection();
		float magnitude = forces.force.GetMagnitude();

		serializer.ReadObject(forces.mass, entityId, "entities.forces.mass");
		serializer.ReadObject(forces.gravityScale, entityId, "entities.forces.gravityScale");
		serializer.ReadObject(forces.jump, entityId, "entities.forces.jump");
		serializer.ReadObject(forces.dampening, entityId, "entities.forces.dampening");
		serializer.ReadObject(forces.velocity, entityId, "entities.forces.velocity");
		serializer.ReadObject(forces.maxVelocity, entityId, "entities.forces.maxVelocity");
		serializer.ReadObject(forces.acceleration, entityId, "entities.forces.acceleration");
		serializer.ReadObject(direction, entityId, "entities.forces.force.direction");
		serializer.ReadObject(magnitude, entityId, "entities.forces.force.magnitude");
		serializer.ReadObject(forces.accumulatedForce, entityId, "entities.forces.accumulatedForce");
		serializer.ReadObject(forces.maxAccumulatedForce, entityId, "entities.forces.maxAccumulatedForce");
		serializer.ReadObject(forces.prevForce, entityId, "entities.forces.prevForces");
		serializer.ReadObject(forces.targetForce, entityId, "entities.forces.targetForce");

		forces.force.SetDirection(direction);
		forces.force.SetMagnitude(magnitude);

		ecsCoordinator.addComponent(entityObj, enemy);
		ecsCoordinator.addComponent(entityObj, movement);
		ecsCoordinator.addComponent(entityObj, forces);

	}
	else if (!strcmp(items[itemIndex], "Player")) {

		AABBComponent aabb{};
		serializer.ReadObject(aabb.left, entityId, "entities.aabb.left");
		serializer.ReadObject(aabb.right, entityId, "entities.aabb.right");
		serializer.ReadObject(aabb.top, entityId, "entities.aabb.top");
		serializer.ReadObject(aabb.bottom, entityId, "entities.aabb.bottom");

		AnimationComponent animation{};
		serializer.ReadObject(animation.isAnimated, entityId, "entities.animation.isAnimated");

		PhysicsComponent forces{};

		myMath::Vector2D direction = forces.force.GetDirection();
		float magnitude = forces.force.GetMagnitude();

		serializer.ReadObject(forces.mass, entityId, "entities.forces.mass");
		serializer.ReadObject(forces.gravityScale, entityId, "entities.forces.gravityScale");
		serializer.ReadObject(forces.jump, entityId, "entities.forces.jump");
		serializer.ReadObject(forces.dampening, entityId, "entities.forces.dampening");
		serializer.ReadObject(forces.velocity, entityId, "entities.forces.velocity");
		serializer.ReadObject(forces.maxVelocity, entityId, "entities.forces.maxVelocity");
		serializer.ReadObject(forces.acceleration, entityId, "entities.forces.acceleration");
		serializer.ReadObject(direction, entityId, "entities.forces.force.direction");
		serializer.ReadObject(magnitude, entityId, "entities.forces.force.magnitude");
		serializer.ReadObject(forces.accumulatedForce, entityId, "entities.forces.accumulatedForce");
		serializer.ReadObject(forces.maxAccumulatedForce, entityId, "entities.forces.maxAccumulatedForce");
		serializer.ReadObject(forces.prevForce, entityId, "entities.forces.prevForces");
		serializer.ReadObject(forces.targetForce, entityId, "entities.forces.targetForce");

		forces.force.SetDirection(direction);
		forces.force.SetMagnitude(magnitude);

		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, animation);
		ecsCoordinator.addComponent(entityObj, forces);

	}
	else if (!strcmp(items[itemIndex], "Platform")) {

		AABBComponent aabb{};
		serializer.ReadObject(aabb.left, entityId, "entities.aabb.left");
		serializer.ReadObject(aabb.right, entityId, "entities.aabb.right");
		serializer.ReadObject(aabb.top, entityId, "entities.aabb.top");
		serializer.ReadObject(aabb.bottom, entityId, "entities.aabb.bottom");

		ClosestPlatform closestPlatform{};
		serializer.ReadObject(closestPlatform.isClosest, entityId, "entities.closestPlatform.isClosest");

		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, closestPlatform);

	}
}
