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
#include <cmath>

bool IsPointInCircle(float pointX, float pointY, float circleX, float circleY, float radius) {
	// Calculate the distance between the point and circle center using distance formula
	float distanceX = pointX - circleX;
	float distanceY = pointY - circleY;

	// Use Pythagorean theorem to get actual distance
	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

	// Compare with radius squared (avoiding square root for better performance)
	return distanceSquared <= (radius * radius);
}

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
bool isSelectingSaveFile;
bool saveFileChosen = false;
bool loadFileChosen = false;
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
char modifyTextBuffer[MAXTEXTSIZE];
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

	AssetBrowser::Initialise();
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

			const char* items[] = { "Player", "Enemy", "Platform", "TextBox", "Background"};

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

			/*JSONSerializer serializer;
			if (!serializer.Open(FilePathManager::GetEntitiesJSONPath())) {
				std::cout << "Error: could not open file " << FilePathManager::GetEntitiesJSONPath() << std::endl;
				return;
			}*/
			Entity entityObj;
			std::string entityId;

			/*nlohmann::json jsonObj = serializer.GetJSONObject();*/
			//Entity creation button functionalities
			if (ImGui::Button("Create Entity")) {

				for (int i = 0; i < numEntitiesToCreate; i++) {

					entityObj = ecsCoordinator.createEntity();

					entityId = sigBuffer;

					TransformComponent transform{};

					transform.position.SetX(xCoordinates);
					transform.position.SetY(yCoordinates);
					transform.scale.SetX(defaultObjScaleX);
					transform.scale.SetY(defaultObjScaleY);
					ecsCoordinator.addComponent(entityObj, transform);
					ecsCoordinator.setEntityID(entityObj, entityId);

					ObjectCreationCondition(items, currentItem, entityObj, entityId);
					newEntities.push_back(entityObj);
					

				}


			}
			ImGui::SameLine();

			//For creating entity with random position
			if (ImGui::Button("Random")) {
				for (int i = 0; i < numEntitiesToCreate; i++) {

					entityObj = ecsCoordinator.createEntity();

					entityId = sigBuffer;

					TransformComponent transform{};

					transform.position = myMath::Vector2D(ecsCoordinator.getRandomVal(coordinateMinLimitsX, coordinateMaxLimitsX),
						ecsCoordinator.getRandomVal(coordinateMinLimitsY, coordinateMaxLimitsY));
					transform.scale.SetX(defaultObjScaleX);
					transform.scale.SetY(defaultObjScaleY);
					ecsCoordinator.addComponent(entityObj, transform);
					ecsCoordinator.setEntityID(entityObj, entityId);

					ObjectCreationCondition(items, currentItem, entityObj, entityId);
					newEntities.push_back(entityObj);


				}
			}

			//Button to destroy all existing entity
			if (ImGui::Button("Remove All Entity")) {
				for (auto entity : ecsCoordinator.getAllLiveEntities()) {
					if (ecsCoordinator.getEntityID(entity) != "placeholderentity") {
						ecsCoordinator.destroyEntity(entity);
					}
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
						size_t maxSize = 1000;
						std::string tempStr = fontComp.text;

						size_t length = std::min(tempStr.length(), maxSize);
						for (size_t i = 0; i < length; i++)
						{
							textBuffer[i] = tempStr[i];
						}
						

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

							ImGui::SetNextItemWidth(objAttributeSliderMaxLength);  // Set width of input field
							ImGui::InputText("##Text", textBuffer, IM_ARRAYSIZE(textBuffer));
							ImGui::SameLine();
							ImGui::Text("Text");
							fontComp.text = textBuffer;
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
			if (ImGui::Button("Save")) {
				isSelectingSaveFile = true;
			}

			if (isSelectingSaveFile) {
				ImGui::OpenPopup("Choose save files");
			}


			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			// Create popup modal window for loading of save files
			if (ImGui::BeginPopupModal("Choose save files", &isSelectingSaveFile, ImGuiWindowFlags_AlwaysAutoResize)) {

				ImGui::BeginChild("SaveFilesList", ImVec2(saveWindowWidth, saveWindowHeight), true);

				int saveNum;
				int fileNum;

				if (ImGui::Button("Slot 1", ImVec2(fileWindowWidth - 80, fileWindowHeight))) {
					saveNum = 1;
					saveFileChosen = true;
					isSelectingSaveFile = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear 1", ImVec2(80, fileWindowHeight))) {
					fileNum = 1;
					std::string saveFile = GenerateSaveJSONFile(fileNum);
					nlohmann::ordered_json jsonData;
					jsonData["entities"] = nlohmann::ordered_json::array();
					jsonData["entities"].push_back(nlohmann::ordered_json{
						{"id", "placeholderentity"}
						});
					std::ofstream outputFile(saveFile);
					if (outputFile.is_open()) {
						outputFile << jsonData.dump(2);
						outputFile.close();
					}
				}

				if (ImGui::Button("Slot 2", ImVec2(fileWindowWidth - 80, fileWindowHeight))) {
					saveNum = 2;
					saveFileChosen = true;
					isSelectingSaveFile = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear 2", ImVec2(80, fileWindowHeight))) {
					fileNum = 2;
					std::string saveFile = GenerateSaveJSONFile(fileNum);
					nlohmann::ordered_json jsonData;
					jsonData["entities"] = nlohmann::ordered_json::array();
					jsonData["entities"].push_back(nlohmann::ordered_json{
						{"id", "placeholderentity"}
						});
					std::ofstream outputFile(saveFile);
					if (outputFile.is_open()) {
						outputFile << jsonData.dump(2);
						outputFile.close();
					}
				}

				if (ImGui::Button("Slot 3", ImVec2(fileWindowWidth - 80, fileWindowHeight))) {
					saveNum = 3;
					saveFileChosen = true;
					isSelectingSaveFile = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear 3", ImVec2(80, fileWindowHeight))) {
					fileNum = 3;
					std::string saveFile = GenerateSaveJSONFile(fileNum);
					nlohmann::ordered_json jsonData;
					jsonData["entities"] = nlohmann::ordered_json::array();
					jsonData["entities"].push_back(nlohmann::ordered_json{
						{"id", "placeholderentity"}
						});
					std::ofstream outputFile(saveFile);
					if (outputFile.is_open()) {
						outputFile << jsonData.dump(2);
						outputFile.close();
					}
				}

				if (ImGui::Button("Slot 4", ImVec2(fileWindowWidth - 80, fileWindowHeight))) {
					saveNum = 4;
					saveFileChosen = true;
					isSelectingSaveFile = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear 4", ImVec2(80, fileWindowHeight))) {
					fileNum = 4;
					std::string saveFile = GenerateSaveJSONFile(fileNum);
					nlohmann::ordered_json jsonData;
					jsonData["entities"] = nlohmann::ordered_json::array();
					jsonData["entities"].push_back(nlohmann::ordered_json{
						{"id", "placeholderentity"}
						});
					std::ofstream outputFile(saveFile);
					if (outputFile.is_open()) {
						outputFile << jsonData.dump(2);
						outputFile.close();
					}
				}

				if (ImGui::Button("Slot 5", ImVec2(fileWindowWidth - 80, fileWindowHeight))) {
					saveNum = 5;
					saveFileChosen = true;
					isSelectingSaveFile = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear 5", ImVec2(80, fileWindowHeight))) {
					fileNum = 5;
					std::string saveFile = GenerateSaveJSONFile(fileNum);
					nlohmann::ordered_json jsonData;
					jsonData["entities"] = nlohmann::ordered_json::array();
					jsonData["entities"].push_back(nlohmann::ordered_json{
						{"id", "placeholderentity"}
						});
					std::ofstream outputFile(saveFile);
					if (outputFile.is_open()) {
						outputFile << jsonData.dump(2);
						outputFile.close();
					}
				}


				if (saveFileChosen) {
					std::string saveFile = GenerateSaveJSONFile(saveNum);

					// Create base JSON structure using ordered_json consistently
					nlohmann::ordered_json jsonData;
					jsonData["entities"] = nlohmann::ordered_json::array();

					// Add placeholder entity using ordered_json
					jsonData["entities"].push_back(nlohmann::ordered_json{
						{"id", "placeholderentity"}
						});

					// Save all currently live entities
					for (auto entity : ecsCoordinator.getAllLiveEntities()) {
						std::string entityId = ecsCoordinator.getEntityID(entity);
						if (entityId != "placeholderentity") {
							TransformComponent transform = ecsCoordinator.getComponent<TransformComponent>(entity);
							auto entityJson = DebugSystem::AddNewEntityToJSON(transform, entityId, ecsCoordinator, entity);
							jsonData["entities"].push_back(entityJson);
						}
					}

					// Clear newEntities since they're already saved
					newEntities.clear();

					// Save to file
					std::ofstream outputFile(saveFile);
					if (outputFile.is_open()) {
						outputFile << jsonData.dump(2);
						outputFile.close();
					}

					//saveCount++;
					SaveDebugConfigFromJSON(FilePathManager::GetIMGUIDebugJSONPath());
					saveFileChosen = false;
					isSelectingSaveFile = false;
					//ImGui::CloseCurrentPopup();
				}


				ImGui::EndChild();

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Load")) {
				isSelectingFile = true;
			}

			if (isSelectingFile) {
				ImGui::OpenPopup("Load save files");
			}

			ImVec2 saveCenter = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(saveCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			// Create popup modal window for loading of save files
			if (ImGui::BeginPopupModal("Load save files", &isSelectingFile, ImGuiWindowFlags_AlwaysAutoResize)) {

				ImGui::BeginChild("SaveFilesList", ImVec2(saveWindowWidth, saveWindowHeight), true);

				if (ImGui::Button("Original File", ImVec2(fileWindowWidth, fileWindowHeight))) {

					for (auto entity : ecsCoordinator.getAllLiveEntities()) {
						ecsCoordinator.destroyEntity(entity);
					}
					ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetEntitiesJSONPath());
					isSelectingFile = false;
					ImGui::CloseCurrentPopup();
				}

				int saveNum;

				if (ImGui::Button("Save 1", ImVec2(fileWindowWidth, fileWindowHeight))) {
					saveNum = 1;
					loadFileChosen = true;
					isSelectingFile = false;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Save 2", ImVec2(fileWindowWidth, fileWindowHeight))) {
					saveNum = 2;
					loadFileChosen = true;
					isSelectingFile = false;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Save 3", ImVec2(fileWindowWidth, fileWindowHeight))) {
					saveNum = 3;
					loadFileChosen = true;
					isSelectingFile = false;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Save 4", ImVec2(fileWindowWidth, fileWindowHeight))) {
					saveNum = 4;
					loadFileChosen = true;
					isSelectingFile = false;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Save 5", ImVec2(fileWindowWidth, fileWindowHeight))) {
					saveNum = 5;
					loadFileChosen = true;
					isSelectingFile = false;
					ImGui::CloseCurrentPopup();
				}

				if (loadFileChosen) {
					for (auto entity : ecsCoordinator.getAllLiveEntities()) {
						ecsCoordinator.destroyEntity(entity);
					}

					ecsCoordinator.LoadEntityFromJSON(ecsCoordinator, FilePathManager::GetSaveJSONPath(saveNum));
					loadFileChosen = false;
				}

				ImGui::EndChild();

				ImGui::EndPopup();
			}
		}

		ImGui::End();

		ImGui::Begin("Game Viewport", nullptr, ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse);
		GameViewWindow::Update(); //Game viewport system
		ImGui::End();

		ImVec2 mouseWorldPos = GameViewWindow::GetMouseWorldPosition();
		static int selectedEntityID = -1; // -1 means no entity is selected
		static int draggedEntityID = -1;  // -1 means no entity is being dragged
		static int chosenEntityID = -1;   // Entity selected for potential deletion
		bool openDeletePopup = false;

		// Inspector Window
		if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove)) {
			if (selectedEntityID != -1) {
				std::string entityID = ecsCoordinator.getEntityID(selectedEntityID);

				// Only show inspector for non-background entities
				if (entityID != "placeholderentity") {
					ImGui::Text("Entity: %s", entityID.c_str());
					ImGui::Separator();

					auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);

					// Create local variables for the sliders
					float posXSlide = transform.position.GetX();
					float posYSlide = transform.position.GetY();
					float orientationSlide = transform.orientation.GetX();
					objWidthSlide = transform.scale.GetX();
					objHeightSlide = transform.scale.GetY();

					// Create a unique ID stack for this entity
					ImGui::PushID(entityID.c_str());  // Use string ID instead of numeric ID

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
						ecsCoordinator.destroyEntity(selectedEntityID);
						selectedEntityID = -1;  // Reset selection after deletion
					}

					ImGui::PopID();
					ImGui::Separator();
				}
			}
			else {
				ImGui::Text("No entity selected");
			}
		}
		ImGui::End();

		// Function to check if mouse is over entity
		auto isMouseOverEntity = [&](int entity, float& outDistSq) -> bool {
			auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
			float radiusX = transform.scale.GetX() / 2.0f;
			float radiusY = transform.scale.GetY() / 2.0f;
			float dx = mouseWorldPos.x - transform.position.GetX();
			float dy = mouseWorldPos.y - transform.position.GetY();
			outDistSq = dx * dx + dy * dy;
			return outDistSq <= (radiusX * radiusY);
			};

		// Handle entity selection
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			bool entityFound = false;
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				// Skip background AND placeholder entities during selection
				if (ecsCoordinator.getEntityID(entity) != "background" &&
					ecsCoordinator.getEntityID(entity) != "placeholderentity") {
					float distSq;
					if (isMouseOverEntity(entity, distSq)) {
						selectedEntityID = entity;
						draggedEntityID = entity;
						entityFound = true;
						break;
					}
				}
			}
			if (!entityFound) {
				selectedEntityID = -1;
			}
		}

		// Handle entity dragging
		if (draggedEntityID != -1) {
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				auto& transform = ecsCoordinator.getComponent<TransformComponent>(draggedEntityID);
				transform.position.SetX(mouseWorldPos.x);
				transform.position.SetY(mouseWorldPos.y);
			}
			else {
				draggedEntityID = -1;
			}
		}

		// Handle right-click deletion
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				if (ecsCoordinator.getEntityID(entity) != "background") {
					float distSq;
					if (isMouseOverEntity(entity, distSq)) {
						chosenEntityID = entity;
						openDeletePopup = true;
						break;
					}
				}
			}
		}

		// Handle mouse wheel scaling
		if (ImGui::GetIO().MouseWheel != 0.0f) {
			float wheel_delta = ImGui::GetIO().MouseWheel;
			for (auto entity : ecsCoordinator.getAllLiveEntities()) {
				if (ecsCoordinator.getEntityID(entity) != "background") {
					auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
					float radiusX = transform.scale.GetX() / 2.0f;
					float radiusY = transform.scale.GetY() / 2.0f;
					float dx = mouseWorldPos.x - transform.position.GetX();
					float dy = mouseWorldPos.y - transform.position.GetY();
					float distSq = dx * dx + dy * dy;
					float scaleFactor = 1.0f + (wheel_delta * 0.1f);

					if (distSq <= (radiusX * radiusY)) {
						// If CTRL is held, adjust width (X scale)
						if (ImGui::GetIO().KeyCtrl) {
							transform.scale.SetX(transform.scale.GetX() * scaleFactor);
						}
						// Otherwise adjust height (Y scale)
						else {
							transform.scale.SetY(transform.scale.GetY() * scaleFactor);
						}
					}
				}
			}
		}

		// Create and handle the delete confirmation popup
		if (openDeletePopup) {
			ImGui::OpenPopup("Delete Entity?");
			openDeletePopup = false;
		}

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Delete Entity?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Are you sure you want to delete this entity?");
			ImGui::Separator();

			if (ImGui::Button("Delete", ImVec2(120, 0))) {
				if (chosenEntityID != -1) {
					if (chosenEntityID == selectedEntityID) {
						selectedEntityID = -1;
					}
					if (chosenEntityID == draggedEntityID) {
						draggedEntityID = -1;
					}
					ecsCoordinator.destroyEntity(chosenEntityID);
					chosenEntityID = -1;
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				chosenEntityID = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::Begin("Console");
		Console::Update("Console"); //ImGui console system
		ImGui::End();

		ImGui::Begin("Assets Browser");
		AssetBrowser::Update();
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
nlohmann::ordered_json DebugSystem::AddNewEntityToJSON(TransformComponent& transform, std::string const& entityId, ECSCoordinator& ecs, Entity& entity)
{
	// Initialize ordered components that should always be present first
	nlohmann::ordered_json entityJSON = {
		{"id", entityId},
		{"transform", {
			{"localTransform", {
				{transform.mdl_xform.GetMatrixValue(0, 0), transform.mdl_xform.GetMatrixValue(0, 1), transform.mdl_xform.GetMatrixValue(0, 2)},
				{transform.mdl_xform.GetMatrixValue(1, 0), transform.mdl_xform.GetMatrixValue(1, 1), transform.mdl_xform.GetMatrixValue(1, 2)},
				{transform.mdl_xform.GetMatrixValue(2, 0), transform.mdl_xform.GetMatrixValue(2, 1), transform.mdl_xform.GetMatrixValue(2, 2)}
			}},
			{"orientation", {
				{"x", transform.orientation.GetX()},
				{"y", transform.orientation.GetY()}
			}},
			{"position", {
				{"x", transform.position.GetX()},
				{"y", transform.position.GetY()}
			}},
			{"projectionMatrix", {
				{transform.mdl_to_ndc_xform.GetMatrixValue(0, 0), transform.mdl_to_ndc_xform.GetMatrixValue(0, 1), transform.mdl_to_ndc_xform.GetMatrixValue(0, 2)},
				{transform.mdl_to_ndc_xform.GetMatrixValue(1, 0), transform.mdl_to_ndc_xform.GetMatrixValue(1, 1), transform.mdl_to_ndc_xform.GetMatrixValue(1, 2)},
				{transform.mdl_to_ndc_xform.GetMatrixValue(2, 0), transform.mdl_to_ndc_xform.GetMatrixValue(2, 1), transform.mdl_to_ndc_xform.GetMatrixValue(2, 2)}
			}},
			{"scale", {
				{"x", transform.scale.GetX()},
				{"y", transform.scale.GetY()}
			}}
		}}
	};

	// Add additional components in the desired order
	if (ecs.hasComponent<AABBComponent>(entity)) {
		auto& aabb = ecs.getComponent<AABBComponent>(entity);
		entityJSON["aabb"] = {
			{"left", aabb.left},
			{"right", aabb.right},
			{"top", aabb.top},
			{"bottom", aabb.bottom}
		};
	}

	if (ecs.hasComponent<PhysicsComponent>(entity)) {
		auto& rb = ecs.getComponent<PhysicsComponent>(entity);
		entityJSON["forces"] = {
			{"mass", rb.mass},
			{"gravityScale", {
				{"x", rb.gravityScale.GetX()},
				{"y", rb.gravityScale.GetY()}
			}},
			{"jump", rb.jump},
			{"dampening", rb.dampening},
			{"velocity", {
				{"x", rb.velocity.GetX()},
				{"y", rb.velocity.GetY()}
			}},
			{"maxVelocity", rb.maxVelocity},
			{"acceleration", {
				{"x", rb.acceleration.GetX()},
				{"y", rb.acceleration.GetY()}
			}},
			{"force", {
				{"direction", {
					{"x", rb.force.GetDirection().GetX()},
					{"y", rb.force.GetDirection().GetY()}
				}},
				{"magnitude", rb.force.GetMagnitude()}
			}},
			{"accumulatedForce", {
				{"x", rb.accumulatedForce.GetX()},
				{"y", rb.accumulatedForce.GetY()}
			}},
			{"maxAccumulatedForce", rb.maxAccumulatedForce},
			{"prevForce", rb.prevForce},
			{"targetForce", rb.targetForce}
		};
	}

	if (ecs.hasComponent<EnemyComponent>(entity)) {
		entityJSON["enemy"] = { {"isEnemy", true} };
	}

	if (ecs.hasComponent<ClosestPlatform>(entity)) {
		auto& platform = ecs.getComponent<ClosestPlatform>(entity);
		entityJSON["closestPlatform"] = { {"isClosest", platform.isClosest} };
	}

	if (ecs.hasComponent<AnimationComponent>(entity)) {
		entityJSON["animation"] = { {"isAnimated", true} };
	}

	if (ecs.hasComponent<FontComponent>(entity)) {
		auto& fontComp = ecs.getComponent<FontComponent>(entity);
		entityJSON["font"] = nlohmann::ordered_json{
			{"text", {
				{"string", fontComp.text},
				{"BoxWidth", fontComp.textBoxWidth}
			}},
			{"textScale", {
				{"scale", fontComp.textScale}
			}},
			{"color", {
				{"x", fontComp.color.GetX()},
				{"y", fontComp.color.GetY()},
				{"z", fontComp.color.GetZ()}
			}},
			{"fontId", {
				{"fontName", fontComp.fontId}
			}}
		};
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
	std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\Sandbox\\assets\\json\\save" + std::to_string(saveNumber) + ".json";
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
	if (!outFile.is_open()) {
		std::cout << "Error: could not create file " << jsonPath << std::endl;
		return "";
	}
	outFile.close();

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
void DebugSystem::ObjectCreationCondition(const char* items[], int itemIndex, Entity entityObj, std::string entityId) {

	// Get transform for AABB calculations
	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entityObj);

	if (!strcmp(items[itemIndex], "Enemy")) {
		// For a new enemy entity, set default values
		EnemyComponent enemy{};
		enemy.isEnemy = true;  // Set default for new enemy

		MovementComponent movement{};
		movement.speed = 5.0f;  // Default speed

		PhysicsComponent forces{};
		forces.mass = 1.0f;
		forces.gravityScale = { -0.98f, -0.98f };
		forces.jump = 0.8f;
		forces.dampening = 0.9f;
		forces.maxVelocity = 0.0f;
		forces.force.SetDirection({ 0.0f, 0.0f });
		forces.force.SetMagnitude(0.05f);

		// Calculate AABB based on transform
		AABBComponent aabb{};
		float halfWidth = transform.scale.GetX() / 2.0f;
		float halfHeight = transform.scale.GetY() / 2.0f;
		aabb.left = transform.position.GetX() - halfWidth;
		aabb.right = transform.position.GetX() + halfWidth;
		aabb.top = transform.position.GetY() + halfHeight;
		aabb.bottom = transform.position.GetY() - halfHeight;

		ecsCoordinator.addComponent(entityObj, enemy);
		ecsCoordinator.addComponent(entityObj, movement);
		ecsCoordinator.addComponent(entityObj, forces);
		ecsCoordinator.addComponent(entityObj, aabb);
	}
	else if (!strcmp(items[itemIndex], "Player")) {

		AnimationComponent animation{};
		animation.isAnimated = true;

		// Calculate AABB based on transform
		AABBComponent aabb{};
		float halfWidth = transform.scale.GetX() / 2.0f;
		float halfHeight = transform.scale.GetY() / 2.0f;
		aabb.left = transform.position.GetX() - halfWidth;
		aabb.right = transform.position.GetX() + halfWidth;
		aabb.top = transform.position.GetY() + halfHeight;
		aabb.bottom = transform.position.GetY() - halfHeight;

		PhysicsComponent forces{};
		forces.mass = 1.0f;
		forces.gravityScale = { 9.8f, 9.8f };
		forces.jump = 0.0f;
		forces.dampening = 0.9f;
		forces.maxVelocity = 200.0f;
		forces.force.SetDirection({ 0.0f, 0.0f });
		forces.force.SetMagnitude(10.0f);
		forces.maxAccumulatedForce = 40.0f;

		ecsCoordinator.addComponent(entityObj, animation);
		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, forces);
	}
	else if (!strcmp(items[itemIndex], "Platform")) {
		// Calculate AABB based on transform
		AABBComponent aabb{};
		float halfWidth = transform.scale.GetX() / 2.0f;
		float halfHeight = transform.scale.GetY() / 2.0f;
		aabb.left = transform.position.GetX() - halfWidth;
		aabb.right = transform.position.GetX() + halfWidth;
		aabb.top = transform.position.GetY() + halfHeight;
		aabb.bottom = transform.position.GetY() - halfHeight;

		ClosestPlatform closestPlatform{};
		closestPlatform.isClosest = (transform.orientation.GetX() == 0); // Set true only for flat platform

		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, closestPlatform);
	}
	else if (!strcmp(items[itemIndex], "TextBox")) {
		// Calculate AABB based on transform
		FontComponent fontComp{};
		fontComp.text = textBuffer;
		fontComp.textScale = textScale;
		fontComp.fontId = "Journey";
		fontComp.textBoxWidth = textBoxMinLimit;
		fontComp.color.SetX(1);
		fontComp.color.SetY(2);
		fontComp.color.SetZ(3);

		ecsCoordinator.addComponent(entityObj, fontComp);
	}
	else if (!strcmp(items[itemIndex], "Background")) {
		
	}
}
