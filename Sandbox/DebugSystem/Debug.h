/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Debug.h
@brief  :  This file contains the function declaration of ImGui GUI debugging features which include the debug
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
#pragma once
#include "EngineDefinitions.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
//#include <filesystem>
#include "GlfwFunctions.h"

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

	static int GetSaveCount();

	nlohmann::json AddNewEntityToJSON(TransformComponent& transform, std::string const& entityId);
	void RemoveEntityFromJSON(std::string const& entityId);

	void LoadDebugConfigFromJSON(std::string const& filename);

	void ObjectCreationCondition(const char* items[], int current_item, JSONSerializer& serializer, Entity entityObj, std::string entityId, char* textBuffer);
	
	void SaveDebugConfigFromJSON(std::string const& filename);

	std::string GenerateSaveJSONFile(int& saveNumber);

private:
	ImGuiIO* io;
	ImFont* font;
	static float fontSize;
	static float textBorderSize;
	static ImVec4 clearColor;

	static bool isZooming;
	static bool isPanning;
	static bool isSliding;
	static bool isRedToggled;
	static bool isGreenToggled;
	static bool isBlueToggled;

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

	static int numEntitiesToCreate;
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

	static int saveCount;
	static float lastPosX;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key); //Prevent key duplication according to ImGui legacy key map
