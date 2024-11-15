/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Debug.h
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
#pragma once
#include "EngineDefinitions.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "GlfwFunctions.h"

#include "ECSCoordinator.h"
#include "Systems.h"
#include "matrix3x3.h"
#include "GraphicsSystem.h"
#include "WindowSystem.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "../Serialization/jsonSerialization.h"

#define MAXNAMELENGTH 15
#define MAXBUFFERSIZE 8
#define MAXTEXTSIZE 1000
#define MAXOBJTYPE 4

//Class for the ImGui GUI debugbug mode which displays debug, gameviewport, and console window
class DebugSystem : public GameSystems/*, public System*/ {
public:

	DebugSystem();
	~DebugSystem();

	void GameSystems::initialise() override;
	//void System::initialise() override;

	//void System::update(float dt) override;
	void GameSystems::update() override;

	void GameSystems::cleanup() override;
	//void System::cleanup() override;

	SystemType getSystem() override;

	void StartLoop(); //Start record game loop time

	void StartSystemTiming(const char* systemName); //Start record system loop time

	void EndSystemTiming(const char* systemName); //End record system loop time

	void EndLoop();  //End record game loop time

	void UpdateSystemTimes(); //Update all system loop time

	static int GetSaveCount(); //Returns save count

	//Serialize entity data to JSON file
	nlohmann::ordered_json AddNewEntityToJSON(TransformComponent& transform, std::string const& entityId, ECSCoordinator& ecs, Entity& entity);

	//Remove entity data from JSON save file
	void RemoveEntityFromJSON(std::string const& entityId);

	//Load debug system configuration from JSON file
	void LoadDebugConfigFromJSON(std::string const& filename);

	//Create appropriate components based on object type
	void ObjectCreationCondition(const char* items[], int current_item, Entity entityObj, std::string entityId);

	//Save current debug save count configuration to JSON
	void SaveDebugConfigFromJSON(std::string const& filename);

	//Generate new JSON save file
	std::string GenerateSaveJSONFile(int& saveNumber);
	
private:
	ImGuiIO* io;
	ImFont* font;
	static float fontSize;
	static float textBorderSize;
	static ImVec4 clearColor;

	static bool isSelectingFile;
	static bool isZooming;
	static bool isPanning;
	static bool isSliding;
	static bool isRedToggled;
	static bool isGreenToggled;
	static bool isBlueToggled;
	static bool createEntity;
	static int currentItem;

	static double ecsTotal;
	static bool foundECS;
	static int numberOfColumnPV;
	static float paddingPV;
	static float initialZoomLevel;
	static float initialDragDistX;
	static float initialDragDistY;

	static float defaultObjScaleX;
	static float defaultObjScaleY;

	static float coordinateMaxLimitsX;
	static float coordinateMaxLimitsY;
	static float coordinateMinLimitsX;
	static float coordinateMinLimitsY;
	static float orientationMaxLimit;
	static float orientationMinLimit;

	static float fontScaleMaxLimit;
	static float fontScaleMinLimit;
	static float fontColorMaxLimit;
	static float fontColorMinLimit;
	static float textBoxMaxLimit;
	static float textBoxMinLimit;
	static float textBoxWidth;

	static int numEntitiesToCreate;
	static int minEntitesToCreate;
	static char numBuffer[MAXBUFFERSIZE];
	static char sigBuffer[MAXNAMELENGTH];
	static char textBuffer[MAXTEXTSIZE];
	static char textScaleBuffer[MAXBUFFERSIZE];
	static char xCoordinatesBuffer[MAXBUFFERSIZE];
	static char yCoordinatesBuffer[MAXBUFFERSIZE];
	static char xOrientationBuffer[MAXBUFFERSIZE];
	static char yOrientationBuffer[MAXBUFFERSIZE];
	static float xCoordinates;
	static float yCoordinates;
	static float xOrientation;
	static float yOrientation;
	static float textScale;
	static float textScaleMinLimit;

	static float objAttributeSliderMaxLength;
	static float objAttributeSliderMidLength;
	static float objAttributeSliderMinLength;

	static std::unordered_map<const char*, double> systemTimes;
	static std::unordered_map<std::string, double> systemStartTimes;
	static std::unordered_map<std::string, double> systemTotalTimes;
	static std::unordered_map<std::string, double> accumulatedTimes;
	static double loopStartTime;
	static double totalLoopTime;
	static double totalECSTime;
	static double lastUpdateTime;
	static bool firstFrame;
	static std::vector<const char*> systems;
	static std::vector<double> systemGameLoopPercent;
	static int systemCount;

	static float objWidthSlide;
	static float objHeightSlide;
	static int objCount;

	static float objSizeXMax;
	static float objSizeXMin;
	static float objSizeYMax;
	static float objSizeYMin;

	static int saveLimit;
	static int saveCount;
	static float lastPosX;
	static float fileWindowWidth;
	static float saveWindowHeight;
	static float fileWindowHeight;
	static float saveWindowWidth;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key); //Prevent key duplication according to ImGui legacy key map